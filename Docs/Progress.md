# fuctory — 実装進捗管理

> このファイルは実装タスクの進捗を管理するためのものです。
> 設計・仕様の詳細は [`GamePlot.md`](./GamePlot.md) を参照してください。
> 作業を進めるたびに、このファイルのチェック状態を更新してください。

最終更新: 2026-07-19(GamePlot.md確定に伴い更新)

## 1. コアシステム(C++)

| 項目 | ファイル | 状態 |
|---|---|---|
| ゲーム型定義(Enum/Struct) | `Core/FuctoryGameTypes.h` | ✅ 完了 |
| EconomySubsystem(資金管理) | `Economy/EconomySubsystem.*` | ✅ 完了 |
| PhaseSubsystem(フェーズ進行・隠れ条件・目標判定) | `Phase/PhaseSubsystem.*` | ✅ 完了 |
| ChoiceSubsystem(選択肢提示・選択・完了管理) | `Choice/ChoiceSubsystem.*` | ✅ 完了 |
| FuctoryGameMode(サブシステム初期化・スポーン設定) | `Core/FuctoryGameMode.*` | ✅ 完了 |
| FuctoryPlayerController(入力・UIフック) | `Player/FuctoryPlayerController.*` | ✅ 完了 |
| IsometricPawn(カメラ/移動) | `Player/IsometricPawn.*` | ⚠️ 実装済みだが動作未検証 |

## 2. データアセット

| 項目 | 必要数 | 現状 | 状態 |
|---|---|---|---|
| PhaseData(建設期) | `DA_Phase_Construction` | 作成済み(数値は`GamePlot.md`6章に未反映) | ⚠️ 数値の再調整必要 |
| PhaseData(立ち上げ期) | `DA_Phase_Setup` | 未作成 | ❌ 未着手 |
| PhaseData(量産期) | `DA_Phase_MassProduction` | 未作成 | ❌ 未着手 |
| ChoiceData(施工業者選定) | `DA_Choice_Contractor` | 作成済み・表示確認済みだが数値は`GamePlot.md`6章の値に未調整 | ⚠️ 数値の再調整必要 |
| ChoiceData(その他の意思決定、計8個) | 各フェーズ3〜4個 | 未作成 | ❌ 未着手(内容は`GamePlot.md`6章で確定済み) |
| 各Objectives / HiddenConditionsの数値設計 | 全フェーズ分 | 確定済み | ✅ 完了(`GamePlot.md`6〜7章) |

## 3. UI

### WBP_ChoiceDialog(選択肢ダイアログ)

| 項目 | 状態 |
|---|---|
| Designerレイアウト(Title/Description/OptionButton_0-2) | ✅ 完了 |
| ボタン0〜2のテキスト表示ロジック(Sequence + Array Get + IsValidIndex) | ✅ 完了・PIEで表示確認済み |
| タイトル・説明文のテキスト表示ロジック | ✅ 完了・PIEで表示確認済み |
| 各ボタンの `OnClicked` → `HandleOptionSelected` 経由で `SelectChoiceOption(CurrentChoice, Index)` | ✅ 完了・PIEでクリック動作確認済み |
| 選択後にダイアログを閉じる(`Remove from Parent`) | ✅ 完了(`HandleOptionSelected` 内で同時に実装) |

### PlayerController 側の表示フック

| 項目 | 状態 |
|---|---|
| `BP_ShowChoiceDialog`: Create Widget → Set CurrentChoice → Add to Viewport | ✅ 完了・PIEで表示確認済み |
| `BP_OnPhaseChanged`: フェーズ切り替え演出(`WBP_PhaseBanner` をCreate Widget→Add to Viewport→2.5秒後Remove from Parent) | ✅ 完了 |
| `BP_OnGameOver`: ゲームオーバー画面表示(`WBP_GameOver` にMessageを渡してAdd to Viewport) | ✅ 完了 |
| `BP_OnGameClear`: ゲームクリア画面表示(`WBP_GameClear` をAdd to Viewport) | ✅ 完了 |

### WBP_PhaseBanner(フェーズ切り替えバナー)

| 項目 | 状態 |
|---|---|
| Designerレイアウト(Canvas Panel + PhaseNameText) | ✅ 完了 |
| `PhaseName`(Text, Expose on Spawn)→Construct時に`PhaseNameText`へ反映 | ✅ 完了 |

### WBP_GameOver / WBP_GameClear

| 項目 | 状態 |
|---|---|
| Designerレイアウト(半透明背景+タイトル+メッセージ+ボタン) | ✅ 完了 |
| `GameOverMessage`(Text, Expose on Spawn)→Construct時に反映(GameOverのみ) | ✅ 完了 |
| リトライ/リスタートボタン → `Open Level (by Name)` = `MainLevel` | ✅ 完了 |

### その他のUI(未着手)

| 項目 | 状態 |
|---|---|
| HUD(資金・ステータス・残り日数表示) | ❌ 未着手 |
| フェーズ目標(Objectives)の達成状況表示 | ❌ 未着手 |

## 4. 入力(Enhanced Input)

| 項目 | 状態 |
|---|---|
| `IMC_Default` / `IA_PresentChoice` / `IA_CompletePhase` アセット作成 | ✅ 完了 |
| `BP_FuctoryPlayerController` Class Defaults への割り当て | ✅ 完了(未割り当てが原因で入力無反応になった経緯あり) |
| Eキーでの選択肢提示・Rキーでのフェーズ進行(テスト用) | ✅ 完了・動作確認済み |
| 本番用の入力方式(マウスクリック中心のUIにするか等) | ❌ 未検討 |

## 5. レベル/ワールド

| 項目 | 状態 |
|---|---|
| `MainLevel` 作成・GameMode設定 | ✅ 完了 |
| レベルアート・工場建屋の見た目 | ❌ 未着手/未確認 |

## 6. 次にやること(優先順)

1. ~~**選択肢ボタンのクリック処理**~~ → ✅ 完了(2026-07-19)
2. ~~**選択後の後処理**~~ → ✅ 完了(2026-07-19)
3. ~~**フェーズ切り替え・ゲームオーバー/クリアのUI実装**~~ → ✅ 完了(2026-07-19)
4. ~~**GamePlot.mdの未確定事項を詰める**(数値基準・選択肢バリエーション・UI/UX方針・世界観・リプレイ性)~~ → ✅ 完了(2026-07-19。`GamePlot.md` 全面改訂)
5. **C++側の追加実装**(`GamePlot.md` 参照) ← 次はここから
   - `FFactoryStats::SystemProgress` フィールド追加
   - `PhaseSubsystem::CheckHiddenConditions` のDayLimit判定ロジック改修(日数超過時にObjectives判定→分岐、選択肢を使い切った際の判定追加)
   - `FChoiceOption` から非ゼロの変化のみをテキスト化するヘルパー関数(結果パネル用)
   - ※いずれもヘッダー変更を伴うため、Live Codingではなく通常のフルリコンパイルで反映すること
6. **データアセットの作成・調整**(`GamePlot.md` 6章の数値を反映)
   - `DA_Choice_Contractor` の数値調整
   - 建設期の残り選択肢(安全対策・保険/建設資材調達/システム導入①)、`DA_Phase_Setup`とその選択肢4個、`DA_Phase_MassProduction`とその選択肢3個を新規作成
7. **HUD(`WBP_HUD`)の実装**(残り日数・資金・主要ステータスのリアルタイム表示)
8. **選択後の結果確認パネルの実装**(`WBP_ChoiceDialog`改修。ResultText+変化項目の自動列挙→OKボタンで閉じる)
9. **ランダム変動システムの実装**(`FRandomStream`によるプレイスルー単位の±10%変動)

> 補足: 作業中に `BP_FuctoryPlayerController` のClass Defaults(Enhanced Input参照)がプロジェクト再起動のたびに空になる事象が発生。原因はLive Codingでヘッダー変更を反映した際のCDO不整合。`Binaries`/`Intermediate` 削除→完全再コンパイルで解消。今後ヘッダー変更時はLive Codingを使わないこと。

---
*進捗を更新したら、対応する実装ファイル・関連ドキュメント([`GamePlot.md`](./GamePlot.md))とも矛盾がないか確認してください。*
