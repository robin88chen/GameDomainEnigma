TokenVector tokens = split_token(code, "\n\r");

for (unsigned int i = 0; i < tokens.size(); i++)
{
    if (tokens[i].find("//semantic") == 0)
    {
        TokenVector tags = split_token(tokens[i], " \t");
        if (tags.size() >= 3)
        {
            m_semanticTable.insert(make_pair(tags[1], tags[2]));
        }
    }
}
