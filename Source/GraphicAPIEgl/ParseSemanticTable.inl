TokenVector tokens = En_Split_Token(code, "\n\r");
for (unsigned int i = 0; i < tokens.size(); i++)
{
    if (tokens[i].find("//semantic") == 0)
    {
        TokenVector tags = En_Split_Token(tokens[i], " \t");
        if (tags.size() >= 3)
        {
            m_varSemanticTable.insert(make_pair(tags[1], tags[2]));
        }
    }
}
