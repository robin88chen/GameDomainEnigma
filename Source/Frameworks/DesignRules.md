# Design Rules

## Rtti

Rtti 本質是個 Value Object，可以當作 Entity 的標示用。要改設計成可複製。增加 Deriving Map 把 Rtti 基底與衍生資料的串接隔離出來。
Rtti 建構時會使用到 Deriving Map, 所以Rtti不做成靜態資料，避免靜態資料生成順序衝突。

Contract Factory 會用到 Rtti 的名稱字串，需要靜態資料，所以還是需要靜態生成。Deriving 仍舊紀錄在 Rtti物件內。(因為 Deriving Map 沒辦法保證在靜態 Rtti 生成之前先生成)

## Command

Commands 作業方式和 Events 雷同，但設計上意義大不同。拔出一個共同的基底類型也沒太多意義，所以先弄成重複的程式碼。
