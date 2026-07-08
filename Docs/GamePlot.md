# fuctory — ゲームプロット / 設計指針

> このファイルは、クラウド上での作業（Claude Codeなど）を進める際の一貫した指針として、
> ゲームの根幹となるプロット・システム仕様をまとめたものです。
> 新しい会話・別環境で作業を再開する際は、まずこのファイルを参照してください。

## 1. コンセプト概要

プレイヤーは工場経営者（工場長）として、ゼロから工場を立ち上げ、
**建設 → 生産ライン立ち上げ → 量産** という3つのフェーズを経て、
事業を軌道に乗せることを目指す経営シミュレーションゲーム。

各フェーズで提示される選択肢（施工業者の選定、設備投資、資材調達など）を通じて、
資金・品質・人材・生産性といった複数のステータスをバランスさせながら、
期限内にフェーズごとの目標をクリアしていく。

判断を誤ると資金枯渇や品質不足、納期遅延などにより **ゲームオーバー** となり、
すべてのフェーズを乗り越えると **ゲームクリア** となる。

## 2. フェーズ構成

| 順序 | フェーズ (EGamePhase) | 表示名 | 概要 |
|---|---|---|---|
| 1 | `Construction` | 工場建屋建設期 | 工場の建屋を建設する。施工業者の選定などがここでの主な意思決定 |
| 2 | `Setup` | 生産ライン立ち上げ期 | 設備導入・生産ラインの構築を行う |
| 3 | `MassProduction` | 量産期 | 実際の製品を量産し、納品・信用の維持を行う |
| — | `GameOver` | ゲームオーバー | いずれかのゲームオーバー要因に該当した場合の終了状態 |
| — | `GameClear` | ゲームクリア | 全フェーズの目標を達成した場合の終了状態 |

- 各フェーズには **Day Limit（制限日数）** が設定されており、期限内に目標を達成できないと `TimeOver` でゲームオーバーになる
- フェーズごとに `PhaseData` アセット（`DA_Phase_Construction` など）としてデータ化し、`AFuctoryGameMode::PhaseDataAssets` 配列に順序通り登録する

## 3. ステータスシステム（FFactoryStats）

| ステータス | 初期値 | 主な影響範囲 |
|---|---|---|
| Money（資金） | 50,000,000円 | 選択肢のコスト支払い、資金不足でゲームオーバー |
| BuildingQuality（建屋品質） | 0〜100 | 建設期の意思決定で変動。低いとゲームオーバー要因 |
| EquipmentQuality（設備品質） | 0〜100 | 立ち上げ期の意思決定で変動 |
| MaterialQuality（資材品質） | 0〜100 | 調達選択肢で変動。基準未達でゲームオーバー要因 |
| MaterialStock（資材在庫） | — | 量産期の生産継続に影響 |
| WorkerSkill（作業員技術力） | 0〜100 | 人材関連の選択肢で変動 |
| WorkerCount（作業員数） | — | 人員配置の選択肢で変動 |
| ProductionEfficiency（生産効率） | 0〜100 | 量産期の目標達成に直結 |
| ProductQuality（製品品質） | 0〜100 | 顧客満足度・信用評価に波及 |
| CustomerSatisfaction（顧客満足度） | 70 | 信用評価に影響 |
| Reputation（信用評価） | 70 | 低下すると `ReputationCollapse` でゲームオーバー |
| DaysUsed（経過日数） | 0 | フェーズのDay Limitと比較され、超過で `TimeOver` |

**設計原則**: すべてのステータスはトレードオフの関係になるようにする。
（例：安価な施工業者は日数もコストも抑えられるが BuildingQualityDelta が低い、など）
「絶対に正しい選択肢」を作らず、フェーズごとの残り資金・残り日数・目標水準に応じて
最適解が変わるように選択肢の数値を設計する。

## 4. 選択肢システム（ChoiceData / FChoiceOption）

- 1つの `ChoiceData` が1つの意思決定ポイント（例：「施工業者を選ぶ」）を表す
- 各 `ChoiceData` は2〜3個の `FChoiceOption`（選択肢）を持ち、プレイヤーはその中から1つを選ぶ
- 各オプションは以下を持つ：
  - **表示**: OptionText（選択肢名）/ OptionDetail（詳細説明）/ ResultText（選択後の結果テキスト）
  - **コスト**: MoneyCost（金額）/ DaysUsed（消費日数）
  - **効果**: 各ステータスへの Delta（増減量）
- `bIsRequired = true` の選択肢はフェーズクリアに必須
- `ChoiceOrder` で同一フェーズ内の提示順を制御

## 5. ゲームオーバー条件（EGameOverReason）

| 要因 | 発生条件（想定） |
|---|---|
| Bankruptcy（資金不足） | Money が 0 未満になる |
| LowBuildingQuality（建屋品質不足） | BuildingQuality が基準を下回る |
| LowEquipmentQuality（設備品質不足） | EquipmentQuality が基準を下回る |
| LowMaterialQuality（資材品質基準未達） | MaterialQuality が基準を下回る |
| ProductionFailure（生産目標未達） | ProductionEfficiency や ProductQuality が目標未達 |
| DeliveryFailure（納期遅延） | 量産期の納品スケジュールに間に合わない |
| ReputationCollapse（信用失墜） | Reputation が基準を下回る |
| TimeOver（期限超過） | フェーズの Day Limit を超過して目標未達 |

これらは `FHiddenCondition`（隠れゲームオーバー条件）として `PhaseData` に登録し、
選択のたびにチェックされる。プレイヤーには条件の詳細を明示しない「隠れ条件」として運用する
（緊張感を持たせるための意図的な設計）。

## 6. クリア条件

各フェーズの `Objectives`（`FPhaseObjective`）に設定された目標（例：BuildingQuality が
一定値以上）をすべて満たし、かつ Day Limit 内であれば `TryCompletePhase()` でフェーズクリア。
最終フェーズ（量産期）をクリアすると `GameClear`。

## 7. 未確定事項（今後詰めるべきポイント）

- [ ] 各フェーズの Objectives・HiddenConditions の具体的な数値基準
- [ ] 選択肢のバリエーション（施工業者以外にどんな意思決定ポイントを設けるか）
- [ ] 世界観・キャラクター性（プレイヤーキャラ、取引先NPCなどを設定するか）
- [ ] UI/UX（選択肢提示画面、ステータス表示画面のデザイン方針）
- [ ] リプレイ性の設計（乱数要素、複数エンディングの有無など）

---
*このファイルは実装（`Source/fuctory/Phase/PhaseData.h`, `Source/fuctory/Choice/ChoiceData.h`,
`Source/fuctory/Core/FuctoryGameTypes.h`）と同期させて更新してください。*
