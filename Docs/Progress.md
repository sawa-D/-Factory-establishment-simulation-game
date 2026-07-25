# fuctory — 実装進捗管理

> このファイルは実装タスクの進捗を管理するためのものです。
> 設計・仕様の詳細は [`GamePlot.md`](./GamePlot.md) を参照してください。
> 作業を進めるたびに、このファイルのチェック状態を更新してください。

最終更新: 2026-07-26(ランダム変動システム実装完了に伴い更新)

## 1. コアシステム(C++)

| 項目 | ファイル | 状態 |
|---|---|---|
| ゲーム型定義(Enum/Struct) | `Core/FuctoryGameTypes.h` | ✅ 完了(`SystemProgress`/`SystemProgressDelta`追加済み) |
| EconomySubsystem(資金管理) | `Economy/EconomySubsystem.*` | ✅ 完了 |
| PhaseSubsystem(フェーズ進行・隠れ条件・目標判定) | `Phase/PhaseSubsystem.*` | ✅ 完了(DayLimit到達時のObjectives分岐、選択肢枯渇時の判定を追加済み・フルビルド確認済み) |
| ChoiceSubsystem(選択肢提示・選択・完了管理) | `Choice/ChoiceSubsystem.*` | ✅ 完了(`GetOptionChangeSummary`ヘルパー追加済み、完了マークのタイミングを修正) |
| FuctoryGameMode(サブシステム初期化・スポーン設定) | `Core/FuctoryGameMode.*` | ✅ 完了 |
| FuctoryPlayerController(入力・UIフック) | `Player/FuctoryPlayerController.*` | ✅ 完了 |
| IsometricPawn(カメラ/移動) | `Player/IsometricPawn.*` | ⚠️ 実装済みだが動作未検証 |

## 2. データアセット

| 項目 | 必要数 | 現状 | 状態 |
|---|---|---|---|
| PhaseData(建設期) | `DA_Phase_Construction` | `GamePlot.md`6章の数値に調整済み(DayLimit70/CompletionRevenue3000万/Objectives/HiddenConditions/AvailableChoices) | ✅ 完了 |
| PhaseData(立ち上げ期) | `DA_Phase_Setup` | 新規作成済み | ✅ 完了 |
| PhaseData(量産期) | `DA_Phase_MassProduction` | 新規作成済み | ✅ 完了 |
| ChoiceData(施工業者選定) | `DA_Choice_Contractor` | 数値調整済み | ✅ 完了 |
| ChoiceData(その他の意思決定、計9個) | `DA_Choice_Safety`/`Materials`/`SystemDev_Construction`/`Equipment`/`Hiring`/`LineDesign`/`SystemDev_Setup`/`Materials_MP`/`Pace`/`Sales` | 全て新規作成済み | ✅ 完了 |
| 各Objectives / HiddenConditionsの数値設計 | 全フェーズ分 | 確定・反映済み | ✅ 完了(`GamePlot.md`6〜7章) |
| `BP_FuctoryGameMode` の `PhaseDataAssets` 登録 | Construction→Setup→MassProductionの順 | 登録済み | ✅ 完了 |

## 3. UI

### WBP_ChoiceDialog(選択肢ダイアログ)

| 項目 | 状態 |
|---|---|
| Designerレイアウト(Title/Description/OptionButton_0-2) | ✅ 完了 |
| ボタン0〜2のテキスト表示ロジック(Sequence + Array Get + IsValidIndex) | ✅ 完了・PIEで表示確認済み |
| タイトル・説明文のテキスト表示ロジック | ✅ 完了・PIEで表示確認済み |
| 各ボタンの `OnClicked` → `HandleOptionSelected` 経由で `SelectChoiceOption(CurrentChoice, Index)` | ✅ 完了・PIEでクリック動作確認済み |
| 選択後の結果確認パネル(`ResultPanel`/`ResultText`/`ChangeSummaryText`/`Button_OK`、Designer側) | ✅ 完了 |
| `HandleOptionSelected`: `Option.ResultText`→`ResultText`、`GetOptionChangeSummary(Option)`→`ChangeSummaryText`へ反映 | ✅ 完了・PIEで表示確認済み |
| `HandleOptionSelected`: `MainBox`を`Collapsed`、`ResultPanel`を`Visible`に切り替え | ✅ 完了・PIEで動作確認済み |
| `Button_OK`の`OnClicked` → `Remove from Parent`(ダイアログを閉じる) | ✅ 完了・PIEで動作確認済み |

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

### WBP_HUD(常時表示HUD)

| 項目 | 状態 |
|---|---|
| Designerレイアウト(Vertical Box + DaysText/MoneyText/PhaseStatsText) | ✅ 完了 |
| `Get Phase Subsystem`(`PhaseSys`変数、Promote to Variable) | ✅ 完了 |
| `Event Construct` → `Bind Event to OnStatsUpdated`(`RefreshHUD`)+初期表示用の手動呼び出し | ✅ 完了 |
| `RefreshHUD`: 残り日数/DayLimit・資金・フェーズ別主要ステータス(`Switch on EGamePhase`)の表示更新 | ✅ 完了・PIEで表示確認済み |
| `BP_FuctoryPlayerController`: `BP_OnPhaseChanged`先頭での`IsValid`分岐によるHUD初回生成(`Create Widget`→`Add to Viewport`) | ✅ 完了・PIEで動作確認済み |

### その他のUI(未着手)

| 項目 | 状態 |
|---|---|
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
5. ~~**C++側の追加実装**~~ → ✅ 完了(2026-07-19。`SystemProgress`/`SystemProgressDelta`追加、`CheckHiddenConditions`のDayLimit判定ロジック改修、`GetOptionChangeSummary`ヘルパー追加。フルリコンパイルで動作確認済み)
6. ~~**データアセットの作成・調整**~~ → ✅ 完了(2026-07-19。全フェーズのPhaseData/ChoiceData作成、`BP_FuctoryGameMode`への登録まで完了)
7. ~~**HUD(`WBP_HUD`)の実装**~~ → ✅ 完了(2026-07-20。残り日数・資金・フェーズ別主要ステータスのリアルタイム表示、PIEで確認済み)
8. ~~**選択後の結果確認パネルの実装**~~ → ✅ 完了(2026-07-26。`WBP_ChoiceDialog`にResultPanel追加、`HandleOptionSelected`改修でResultText/ChangeSummaryTextへ反映後、Button_OKで閉じる形に変更。PIEで一連の流れを確認済み)
9. ~~**ランダム変動システムの実装**~~ → ✅ 完了(2026-07-26。`ChoiceSubsystem`に`GetVariedOption`/`ResetForNewPlaythrough`追加、`FuctoryGameMode::InitializeGame`でシード再抽選。`WBP_ChoiceDialog`側もArray Getから`Get Varied Option`呼び出しに差し替え。PIEで動作確認済み)

> 補足: 作業中に `BP_FuctoryPlayerController` のClass Defaults(Enhanced Input参照)がプロジェクト再起動のたびに空になる事象が発生。原因はLive Codingでヘッダー変更を反映した際のCDO不整合。`Binaries`/`Intermediate` 削除→完全再コンパイルで解消。今後ヘッダー変更時はLive Codingを使わないこと。
> 補足2: `UGameInstanceSubsystem`派生クラス(`PhaseSubsystem`/`EconomySubsystem`/`ChoiceSubsystem`)は当初`UCLASS()`のみで`BlueprintType`指定が無く、Blueprint側の変数型検索・自動生成`Get Xxx Subsystem`ノードが出てこない問題があった。3クラスとも`UCLASS(BlueprintType)`に修正して解消(2026-07-20)。
> 補足3: タスク8「選択後の結果確認パネル」完了(2026-07-26)。`HandleOptionSelected`の最終形: `SelectChoiceOption`呼び出し→`Get Choice Subsystem`→`Get Varied Option(CurrentChoice, OptionIndex)`で`Option`取得(タスク9対応後)→`Option.ResultText`を`ResultText`へSet Text→`GetOptionChangeSummary(Option)`を`ChangeSummaryText`へSet Text→`MainBox`を`Collapsed`/`ResultPanel`を`Visible`に切り替え。ダイアログを閉じるのは`Button_OK`の`OnClicked`→`Remove from Parent`に変更(選択直後には閉じない)。
> 補足4: タスク9「ランダム変動システム」完了(2026-07-26)。`ChoiceSubsystem::GetVariedOption(Choice, OptionIndex)`が、ChoiceData名+OptionIndex+`PlaythroughSeed`から`FRandomStream`で決定的に1つの変動率(0.9〜1.1倍)を算出し、MoneyCost/DaysUsed/各Deltaへ一律適用して返す(データアセット自体は変更しない)。`SelectOption`はこの変動後の値でステータス適用まで行うため、表示(結果パネル)と実際の適用値が一致する。`PlaythroughSeed`は`FuctoryGameMode::InitializeGame()`(プレイスルー開始点、Retryでも再実行される)で`ChoiceSys->ResetForNewPlaythrough()`により再抽選。ヘッダー変更を伴うため、Live Codingは使わずフルリビルドで対応(このタイミングで`Intermediate/ProjectFiles`欠落・Smart App Controlによるモジュールブロックの2つの環境問題が別途発生し、解消済み)。

---
*進捗を更新したら、対応する実装ファイル・関連ドキュメント([`GamePlot.md`](./GamePlot.md))とも矛盾がないか確認してください。*
