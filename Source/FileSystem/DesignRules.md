# Design Rules

## File Errors

file errors 有比較多的查詢需求，會需要 last error。 考慮到多緒的問題，last errors 避免放在 global static 裡。

每個 file 物件有各自的 last error。
