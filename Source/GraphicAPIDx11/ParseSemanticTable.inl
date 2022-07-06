TokenVector tokens = En_Split_Token(code, "\n\r");
Graphics::IShaderVariable::SemanticNameTable* semantic_table = new Graphics::IShaderVariable::SemanticNameTable();
for (unsigned int i = 0; i < tokens.size(); i++)
{
    if (tokens[i].find("//semantic") == 0)
    {
        TokenVector tags = En_Split_Token(tokens[i], " \t");
        if (tags.size() >= 3)
        {
            semantic_table->insert(make_pair(tags[1], tags[2]));
        }
    }
}
if (semantic_table->empty())
{
    delete semantic_table;
    return nullptr;
}
return semantic_table;
