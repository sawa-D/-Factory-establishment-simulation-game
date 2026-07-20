# fuctory — ゲームプロット / 設計指針

> このファイルは、クラウド上での作業（Claude Codeなど）を進める際の一貫した指針として、
> ゲームの根幹となるプロット・システム仕様をまとめたものです。
> 新しい会話・別環境で作業を再開する際は、まずこのファイルを参照してください。

最終更新: 2026-07-19(数値設計・UI方針・システム導入要素を確定)

## 1. コンセプト概要

プレイヤーは工場経営者（工場長）として、ゼロから工場を立ち上げ、
**建設 → 生産ライン立ち上げ → 量産** という3つのフェーズを経て、
事業を軌道に乗せることを目指す経営シミュレーションゲーム。

各フェーズで提示される選択肢（施工業者の選定、設備投資、資材調達など）を通じて、
資金・品質・人材・生産性といった複数のステータスをバランスさせながら、
期限内にフェーズごとの目標をクリアしていく。

判断を誤ると資金枯渇や品質不足、納期遅延などにより **ゲームオーバー** となり、
すべてのフェーズを乗り越えると **ゲームクリア** となる。

世界観・キャラクター性は**最小限**とする。施工業者・取引先などは選択肢テキスト上の
固有名詞程度にとどめ、プレイヤーキャラクターの背景設定などは作り込まない。

## 2. フェーズ構成

| 順序 | フェーズ (EGamePhase) | 表示名 | DayLimit | CompletionRevenue |
|---|---|---|---|---|
| 1 | `Construction` | 工場建屋建設期 | 70日 | 30,000,000円 |
| 2 | `Setup` | 生産ライン立ち上げ期 | 65日 | 45,000,000円 |
| 3 | `MassProduction` | 量産期 | 90日 | ―(クリアでGameClear) |
| — | `GameOver` | ゲームオーバー | ― | ― |
| — | `GameClear` | ゲームクリア | ― | ― |

- フェーズごとに `PhaseData` アセット（`DA_Phase_Construction` など）としてデータ化し、`AFuctoryGameMode::PhaseDataAssets` 配列に順序通り登録する
- `FFactoryStats` は `DaysUsed` 以外、**フェーズをまたいでリセットされない**。前フェーズの投資が次フェーズの目標達成しやすさに直結する（例: 立ち上げ期の生産ライン設計への投資を怠ると、量産期の生産効率目標が届きにくくなる）

## 3. フェーズ進行ロジック(DayLimitの扱い)

- プレイヤーは1回の選択で `FChoiceOption::DaysUsed` 分だけ日数が経過し、同時にステータスが増減する
- **早期クリアあり**: 目標(`Objectives`)を満たしていれば、DayLimitを待たずにいつでも次のフェーズに進める（「フェーズ完了を試みる」操作）
- **DayLimitはあくまで締切**: 選択の結果 `DaysUsed` が `DayLimit` を超えた瞬間、その時点のステータスで `Objectives` を自動判定する
  - 満たしていれば → 次フェーズへ進む(ギリギリでも間に合えばセーフ)
  - 満たしていなければ → `GameOver`(`TimeOver`)
- **選択肢を使い切った場合**: そのフェーズの選択肢をすべて選び終えてもなお `Objectives` を満たしていない場合も `GameOver` とする(手詰まり防止のため、日数超過を待たずに判定する)
- **資金が尽きた場合**: 上記とは独立して、選択肢のコストが支払えない瞬間に即座に `GameOver`(`Bankruptcy`)

> 実装メモ: 現状の `PhaseSubsystem::CheckHiddenConditions` は日数超過時に `Objectives` を確認せず無条件で `TimeOver` にしている。上記ロジックに合わせて「日数超過時にまず `AreObjectivesMet()` を確認し、満たしていれば次フェーズへ」という分岐と、「選択肢を使い切った際の判定」を追加実装する必要がある。

**設計原則**: すべてのステータス・選択肢はトレードオフの関係になるようにする。
「絶対に正しい選択肢」を作らず、日数・資金・目標水準に応じて最適解が変わるように数値を設計する。
各フェーズは、標準的なプレイでDayLimitのほとんどを使い切る（ギリギリ間に合うかどうかの緊張感がある）ように調整する。

## 4. ステータスシステム（FFactoryStats）

| ステータス | 初期値 | 主な影響範囲 |
|---|---|---|
| Money（資金） | 50,000,000円 | 選択肢のコスト支払い、資金不足でゲームオーバー |
| BuildingQuality（建屋品質） | 0〜100 | 建設期の意思決定で変動。低いとゲームオーバー要因 |
| EquipmentQuality（設備品質） | 0〜100 | 立ち上げ期の意思決定で変動 |
| MaterialQuality（資材品質） | 0〜100 | 調達選択肢で変動。基準未達でゲームオーバー要因 |
| MaterialStock（資材在庫） | 0 | 量産期の生産継続に影響 |
| WorkerSkill（作業員技術力） | 0〜100 | 人材関連の選択肢で変動 |
| WorkerCount（作業員数） | 0 | 人員配置の選択肢で変動 |
| ProductionEfficiency（生産効率） | 0〜100 | 量産期の目標達成に直結。立ち上げ期の投資が持ち越される |
| ProductQuality（製品品質） | 0〜100 | 顧客満足度・信用評価に波及 |
| CustomerSatisfaction（顧客満足度） | 70 | 信用評価に影響 |
| Reputation（信用評価） | 70 | 低下すると `ReputationCollapse` でゲームオーバー |
| DaysUsed（経過日数） | 0(フェーズ毎リセット) | フェーズのDay Limitと比較され、超過で判定タイミングが発生 |
| **SystemProgress（生産管理システム導入進捗）** | **0〜100(新規追加)** | 建設期から着手し、立ち上げ期のDayLimitまでに100到達必須。未達は無条件でGameOver要因 |

> 実装メモ: `SystemProgress` は `Source/fuctory/Core/FuctoryGameTypes.h` の `FFactoryStats` にC++で新規追加が必要(通常のフルリコンパイルで反映、Live Coding不可)。

## 5. 選択肢システム（ChoiceData / FChoiceOption）

- 1つの `ChoiceData` が1つの意思決定ポイント（例：「施工業者を選ぶ」）を表す
- 各 `ChoiceData` は2〜3個の `FChoiceOption`（選択肢）を持ち、プレイヤーはその中から1つを選ぶ
- 各オプションは以下を持つ：
  - **表示**: OptionText（選択肢名）/ OptionDetail（詳細説明）/ ResultText（選択後の結果テキスト。UI上の結果パネルで使用）
  - **コスト**: MoneyCost（金額）/ DaysUsed（消費日数）
  - **効果**: 各ステータスへの Delta（増減量）
- `ChoiceOrder` で同一フェーズ内の提示順を制御
- 一部の選択肢（生産管理システム導入など）は、当該フェーズ自体の `Objectives` には直結しないが、**将来のフェーズの `Objectives` に影響する**。スルーしても即座には問題ないが、後のフェーズで手詰まりの原因になり得る

## 6. 各フェーズの詳細設計

### フェーズ1: 建設期（Construction）— DayLimit 70日 / CompletionRevenue 30,000,000円

**Objectives**: BuildingQuality ≥ 60
**HiddenConditions**: BuildingQuality < 20 → `LowBuildingQuality`

| # | 選択肢 | A | B | C |
|---|---|---|---|---|
| ① | 施工業者選定(`DA_Choice_Contractor`) | 格安業者: 500万円/15日/建屋+25 | 標準業者: 1200万円/25日/建屋+45 | 大手優良業者: 2200万円/35日/建屋+65 |
| ② | 安全対策・保険グレード選定 | 最低限: 100万円/5日/建屋+0/信用-5 | 標準: 400万円/10日/建屋+5/信用+5 | 手厚い対策: 800万円/15日/建屋+10/信用+10 |
| ③ | 建設資材の調達方針 | 安価資材: 300万円/5日/建屋+5/資材品質30(上書き) | 標準資材: 700万円/10日/建屋+10/資材品質55(上書き) | 高品質資材: 1300万円/15日/建屋+15/資材品質80(上書き) |
| ④ | 生産管理システム導入(要件定義・設計) | 外部ベンダー委託: 600万円/10日/Progress+25 | 自社+ベンダー混成: 1000万円/15日/Progress+40 | エース人材専任: 1500万円/20日/Progress+55 |

→ 施工業者を安価(A)にすると、他をどう組んでも `BuildingQuality` 目標未達。標準(B)以上が基本線。
→ ④を完全にスルー(0)すると、立ち上げ期でどれだけ頑張っても `SystemProgress` が100に届かない。

### フェーズ2: 立ち上げ期（Setup）— DayLimit 65日 / CompletionRevenue 45,000,000円

**Objectives**: EquipmentQuality ≥ 60 / WorkerSkill ≥ 50 / SystemProgress ≥ 100
**HiddenConditions**: EquipmentQuality < 20 → `LowEquipmentQuality`

| # | 選択肢 | A | B | C |
|---|---|---|---|---|
| ① | 設備投資グレード選定 | 中古設備: 600万円/10日/設備+20 | 標準新品設備: 1400万円/20日/設備+40 | 最新鋭設備: 2500万円/25日/設備+60 |
| ② | 作業員採用・研修方針 | 最低人数採用: 200万円/5日/技術+15 | 標準採用+研修: 600万円/12日/技術+35 | 手厚い採用+専門研修: 1100万円/15日/技術+55 |
| ③ | 生産ライン設計方式 | シンプル設計: 300万円/5日/効率+15 | 標準設計: 700万円/10日/効率+30 | 高度自動化設計: 1300万円/10日/効率+45 |
| ④ | 生産管理システム導入(開発・実装) | 標準スケジュール: 800万円/15日/Progress+40 | 増員して開発加速: 1400万円/20日/Progress+60 | 外部専門チーム投入: 2000万円/25日/Progress+80 |

→ ①②はそれぞれ単独の数値で閾値(60・50)を満たす必要があるため、実質C相当が必須級。ここが資金・日数の最大の山場。
→ ④は建設期での投資量に応じて必要グレードが変わる(建設期で55まで到達済みならBで足りるが、建設期0ならCでも届かない)。

### フェーズ3: 量産期（MassProduction）— DayLimit 90日(最終フェーズ)

**Objectives**: ProductionEfficiency ≥ 60 / ProductQuality ≥ 60 / CustomerSatisfaction ≥ 80
**HiddenConditions**: ProductQuality < 20 → `ProductionFailure` / Reputation < 20 → `ReputationCollapse` / MaterialQuality < 20 → `LowMaterialQuality` / MaterialStock < 10 → `DeliveryFailure`

| # | 選択肢 | A | B | C |
|---|---|---|---|---|
| ① | 資材調達(品質 vs コスト) | 安価資材: 800万円/15日/製品+20/在庫+50/資材品質30(上書き) | 標準資材: 1600万円/25日/製品+35/在庫+80/資材品質55(上書き) | 高級資材: 2800万円/35日/製品+50/在庫+100/資材品質80(上書き) |
| ② | 生産ペース設定(増産 vs 安定生産) | 増産優先(突貫): 500万円/15日/効率+50/製品-5 | バランス生産: 900万円/25日/効率+35/製品+10 | 安定・高品質生産: 1400万円/35日/効率+20/製品+25 |
| ③ | 販売・営業方針(価格設定) | 低価格路線: 0円/10日/満足+25 | 標準価格: 300万円/15日/満足+40/信用+10 | プレミアム価格+ブランディング: 900万円/20日/満足+55/信用+20 |

→ `ProductionEfficiency` は立ち上げ期③の持ち越し分とここでの②が合算されて判定される。②を「安定・高品質生産(C)」にすると効率上昇が小さく、逆に効率目標を落としかねない罠がある。
→ `ProductQuality` は①②の組み合わせ次第で、必ずしも両方C(最高品質)にする必要はない(B+C、C+B、C+Cなど複数の到達パターンがある)。

## 7. ゲームオーバー条件（EGameOverReason）

| 要因 | 発生条件 |
|---|---|
| Bankruptcy（資金不足） | 選択肢のコストが支払えない瞬間に即時発生 |
| LowBuildingQuality（建屋品質不足） | 建設期: BuildingQuality < 20 |
| LowEquipmentQuality（設備品質不足） | 立ち上げ期: EquipmentQuality < 20 |
| LowMaterialQuality（資材品質基準未達） | 量産期: MaterialQuality < 20 |
| ProductionFailure（生産目標未達） | 量産期: ProductQuality < 20、または量産期の目標未達のままDayLimit到達/選択肢を使い切る |
| DeliveryFailure（納期遅延） | 量産期: MaterialStock < 10 |
| ReputationCollapse（信用失墜） | 量産期: Reputation < 20 |
| TimeOver（期限超過） | いずれかのフェーズでDayLimit到達時、または選択肢を使い切った時点でObjectives未達(SystemProgress未達を含む) |

`LowBuildingQuality`/`LowEquipmentQuality`/`LowMaterialQuality`/`ReputationCollapse` は `FHiddenCondition` として `PhaseData` に登録し、
選択のたびに即時チェックされる（プレイヤーには詳細を明示しない「隠れ条件」）。
`TimeOver` はDayLimit到達 or 選択肢を使い切った時点の `Objectives` 判定失敗として扱う（3章参照）。

## 8. UI/UX方針

### 常時表示のHUD(`WBP_HUD`、未実装)
画面端に常に表示し、`PhaseSubsystem::OnStatsUpdated` / `GetRemainingDays()` に連動してリアルタイム更新する:
- 残り日数 / DayLimit
- 資金(Money)
- 現在のフェーズに関連する主要ステータス(例: 建設期はBuildingQuality、立ち上げ期はEquipmentQuality/WorkerSkill/SystemProgress)

### 選択後の結果確認パネル(要改修)
現状 `WBP_ChoiceDialog` はボタンを押すと即座に閉じるが、以下に変更する:
1. ボタン押下 → 即座に閉じず、同ダイアログ内に結果パネルを表示
2. 選択肢の `ResultText`(選択ごとの結果テキスト)を表示
3. 変化した項目だけを自動列挙するテキストを表示(例: 「残り日数: -25日」「建屋品質: +45」。効果0の項目は非表示)
4. 「OK」ボタンで閉じる

> 実装メモ: 「変化した項目だけの自動列挙」はC++側に `FChoiceOption` から非ゼロの変化のみをテキスト化するヘルパー関数を追加して対応する。

### 実装済みのUI(参考)
- `WBP_ChoiceDialog`: 選択肢提示ダイアログ(タイトル/説明/3ボタン)
- `WBP_PhaseBanner`: フェーズ切り替え時に表示名を2.5秒表示するバナー
- `WBP_GameOver` / `WBP_GameClear`: 終了画面(メッセージ+リトライボタン)

## 9. リプレイ性(乱数要素)

各選択肢の数値(MoneyCost / DaysUsed / 主要Delta)に、**プレイスルー開始時に決定される±10%程度のランダム変動**を加える。
- 同一プレイ内では固定(選び直しても数値は変わらない、同じ数値を前提に戦略を立てられる)
- プレイスルーごとに微妙に数値が変わることで、周回時の新鮮さ・毎回微妙に異なる最適解を生む
- 乱数要素・複数エンディングの本格導入(バッドエンド分岐など)は今回のスコープ外とし、将来の拡張候補として保留する

> 実装メモ: `FRandomStream` をプレイスルー単位のシードで初期化し、ChoiceData名+OptionIndex+シードから決定的に変動値を算出する方式を想定(データアセット自体は変更せず、適用時に補正をかける)。

## 10. 残タスク・保留事項

- [x] HUD(`WBP_HUD`)の実装(8章参照、2026-07-20完了)
- [ ] 選択後の結果確認パネルの実装(8章参照、`WBP_ChoiceDialog`改修+C++ヘルパー追加)
- [ ] `SystemProgress` フィールドのC++追加
- [ ] `PhaseSubsystem::CheckHiddenConditions` のDayLimit判定ロジック改修(3章参照)
- [ ] ランダム変動システムの実装(9章参照)
- [ ] `DA_Phase_Setup` / `DA_Phase_MassProduction` および各選択肢のChoiceDataアセット作成(6章の数値を反映)
- [ ] `DA_Choice_Contractor` の既存数値を6章の数値に合わせて調整
- [ ] 実際のプレイテストによる数値の微調整(6章の数値は初期案であり、バランス調整の余地がある)

---
*このファイルは実装（`Source/fuctory/Phase/PhaseData.h`, `Source/fuctory/Choice/ChoiceData.h`,
`Source/fuctory/Core/FuctoryGameTypes.h`）と同期させて更新してください。*
