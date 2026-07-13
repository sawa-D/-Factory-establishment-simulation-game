# fuctory — 実装進捗管理

> このファイルは実装タスクの進捗を管理するためのものです。
> 設計・仕様の詳細は [`GamePlot.md`](./GamePlot.md) を参照してください。
> 作業を進めるたびに、このファイルのチェック状態を更新してください。

最終更新: 2026-07-13

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
| PhaseData(建設期) | `DA_Phase_Construction` | 作成済み | ✅ 完了 |
| PhaseData(立ち上げ期) | `DA_Phase_Setup` | 未作成 | ❌ 未着手 |
| PhaseData(量産期) | `DA_Phase_MassProduction` | 未作成 | ❌ 未着手 |
| ChoiceData(施工業者選定) | `DA_Choice_Contractor` | 作成済み・3択のテキストデータ確認済み | ✅ 完了(表示確認済み) |
| ChoiceData(その他の意思決定) | 複数想定 | 未作成 | ❌ 未着手(バリエーション未検討、`GamePlot.md` 7章参照) |
| 各Objectives / HiddenConditionsの数値設計 | 全フェーズ分 | 未確定 | ❌ 未着手 |

## 3. UI

### WBP_ChoiceDialog(選択肢ダイアログ)

| 項目 | 状態 |
|---|---|
| Designerレイアウト(Title/Description/OptionButton_0-2) | ✅ 完了 |
| ボタン0〜2のテキスト表示ロジック(Sequence + Array Get + IsValidIndex) | ✅ 完了・PIEで表示確認済み |
| タイトル・説明文のテキスト表示ロジック | ✅ 完了・PIEで表示確認済み |
| 各ボタンの `OnClicked` → `SelectChoiceOption(CurrentChoice, Index)` | ❌ 未着手 |
| 選択後にダイアログを閉じる(`Remove from Parent`) | ❌ 未着手 |

### PlayerController 側の表示フック

| 項目 | 状態 |
|---|---|
| `BP_ShowChoiceDialog`: Create Widget → Set CurrentChoice → Add to Viewport | ✅ 完了・PIEで表示確認済み |
| `BP_OnPhaseChanged`: フェーズ切り替え演出 | ❌ 未着手 |
| `BP_OnGameOver`: ゲームオーバー画面表示 | ❌ 未着手 |
| `BP_OnGameClear`: ゲームクリア画面表示 | ❌ 未着手 |

### その他のUI(未着手)

| 項目 | 状態 |
|---|---|
| HUD(資金・ステータス・残り日数表示) | ❌ 未着手 |
| フェーズ目標(Objectives)の達成状況表示 | ❌ 未着手 |
| ゲームオーバー画面 | ❌ 未着手 |
| ゲームクリア画面 | ❌ 未着手 |

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

1. **選択肢ボタンのクリック処理**: `OptionButton_0/1/2` の `OnClicked` → `SelectChoiceOption(CurrentChoice, Index)` 呼び出し
2. **選択後の後処理**: `PhaseSubsystem::ApplyChoiceOption` の呼び出し経路の確認、ダイアログの `Remove from Parent`
3. **フェーズ切り替え・ゲームオーバー/クリアのUI実装**(`BP_OnPhaseChanged` / `BP_OnGameOver` / `BP_OnGameClear`)
4. **残り2フェーズ分のPhaseData/ChoiceDataアセット作成**(`GamePlot.md` の未確定事項と合わせて数値設計)
5. **HUD(資金・ステータス表示)の実装**

---
*進捗を更新したら、対応する実装ファイル・関連ドキュメント([`GamePlot.md`](./GamePlot.md))とも矛盾がないか確認してください。*
