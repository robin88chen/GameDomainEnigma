# Design Rules

## Library 連接

先設計成獨立運作的 Library，以後跟核心接上時，再由核心定義的interface來接，或是定義一個Layer來接。

或是由 Filesystem 操作，不直接跟核心關聯。

### Module

對外只有 AssetPackageFile 物件，其餘算是子物件類型。AssetPackageFile 中的每個操作可以獨力完成。

## Error Codes

有傳回值的操作，不傳回 error code，沒有傳回值，但可能出錯的操作，傳回錯誤碼。

先看看這樣的方式合不合用。

Last Error 的方式多緒的時候會出槌。先不用。仍用原本的規劃。

## 其他

AssetPackageFile 需要用 Factory 建造嗎?? 

維持程式碼一致性，加囉~~~
