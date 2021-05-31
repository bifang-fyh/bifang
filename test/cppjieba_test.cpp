#include "bifang.h"

const char* const DICT_PATH = "configs/dict/jieba.dict.utf8";
const char* const HMM_PATH = "configs/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "configs/dict/user.dict.utf8";
const char* const IDF_PATH = "configs/dict/idf.utf8";
const char* const STOP_WORD_PATH = "configs/dict/stop_words.utf8";

int main(int argc, char* argv[])
{
    cppjieba::Jieba jieba(DICT_PATH,
        HMM_PATH,
        USER_DICT_PATH,
        IDF_PATH,
        STOP_WORD_PATH);
    std::vector<std::string> words;
    std::vector<cppjieba::Word> jiebawords;
    std::string s;
    std::string result;

    s = "他来到了网易杭研大厦";
    std::cout << s << std::endl;
    std::cout << "[demo] Cut With HMM" << std::endl;
    jieba.Cut(s, words, true);
    std::cout << limonp::Join(words.begin(), words.end(), "/") << std::endl;

    std::cout << "[demo] Cut Without HMM " << std::endl;
    jieba.Cut(s, words, false);
    std::cout << limonp::Join(words.begin(), words.end(), "/") << std::endl;

    s = "我来到北京清华大学";
    std::cout << s << std::endl;
    std::cout << "[demo] CutAll" << std::endl;
    jieba.CutAll(s, words);
    std::cout << limonp::Join(words.begin(), words.end(), "/") << std::endl;

    s = "小明硕士毕业于中国科学院计算所，后在日本京都大学深造";
    std::cout << s << std::endl;
    std::cout << "[demo] CutForSearch" << std::endl;
    jieba.CutForSearch(s, words);
    std::cout << limonp::Join(words.begin(), words.end(), "/") << std::endl;

    std::cout << "[demo] Insert User Word" << std::endl;
    jieba.Cut("男默女泪", words);
    std::cout << limonp::Join(words.begin(), words.end(), "/") << std::endl;
    jieba.InsertUserWord("男默女泪");
    jieba.Cut("男默女泪", words);
    std::cout << limonp::Join(words.begin(), words.end(), "/") << std::endl;

    std::cout << "[demo] CutForSearch Word With Offset" << std::endl;
    jieba.CutForSearch(s, jiebawords, true);
    std::cout << jiebawords << std::endl;

    std::cout << "[demo] Lookup Tag for Single Token" << std::endl;
    const int DemoTokenMaxLen = 32;
    char DemoTokens[][DemoTokenMaxLen] = {"拖拉机", "CEO", "123", "。"};
    std::vector<std::pair<std::string, std::string> > LookupTagres(sizeof(DemoTokens) / DemoTokenMaxLen);
    std::vector<std::pair<std::string, std::string> >::iterator it;
    for (it = LookupTagres.begin(); it != LookupTagres.end(); it++)
    {
        it->first = DemoTokens[it - LookupTagres.begin()];
        it->second = jieba.LookupTag(it->first);
    }
    std::cout << LookupTagres << std::endl;

    std::cout << "[demo] Tagging" << std::endl;
    std::vector<std::pair<std::string, std::string> > tagres;
    s = "我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。";
    jieba.Tag(s, tagres);
    std::cout << s << std::endl;
    std::cout << tagres << std::endl;

    std::cout << "[demo] Keyword Extraction" << std::endl;
    const size_t topk = 5;
    std::vector<cppjieba::KeywordExtractor::Word> keywordres;
    jieba.extractor.Extract(s, keywordres, topk);
    std::cout << s << std::endl;
    std::cout << keywordres << std::endl;

    return 0;
}
