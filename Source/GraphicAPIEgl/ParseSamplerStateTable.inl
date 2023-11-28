TokenVector tokens = split_token(code, "\n\r");
for (unsigned int i = 0; i < tokens.size(); i++)
{
    if (tokens[i].find("//sampler_state") == 0)
    {
        TokenVector tags = split_token(tokens[i], " \t");
        if (tags.size() >= 3)
        {
            m_texSamplerTable.insert(make_pair(tags[2], tags[1])); // texture name mapping to sampler name
        }
    }
}
