// concatenadorufftopico
// programita para mezclar copias de "topics" que tengan sus comentarios separados en páginas

# include<iostream>
# include<fstream>
# include<vector>
# include<regex> 
# include<algorithm>
using namespace std;

class pageschema {
public:
    int number;
    string comments;
    int size;
    string route;
    string date;
    string url;
};

void contentFindReplaceAll(std::string & data, std::string toSearch, std::string replaceStr)
{
    size_t pos = data.find(toSearch);
    while( pos != std::string::npos)
    {
        data.replace(pos, toSearch.size(), replaceStr);
        pos =data.find(toSearch, pos + replaceStr.size());
    }
}

int countSubstring(const string& str, const string& sub)
{
    if (sub.length() == 0) return 0;
    int count = 0;
    for (size_t offset = str.find(sub); offset != string::npos;
     offset = str.find(sub, offset + sub.length()))
    {
        ++count;
    }
    return count;
}

int fileServe(string routeold, vector<pageschema> pages)
{
    ifstream thefileold (routeold.c_str());
    string pagecontentold( (istreambuf_iterator<char>(thefileold) ),
                       (istreambuf_iterator<char>()    ) );

    string pagecontentnew = pagecontentold.substr(0, pagecontentold.find("<div class=lista-comentarios>"));
    contentFindReplaceAll(pagecontentnew, "--><meta", "--> <!--Page merged with concatenadorufftopico--> <meta");
    pagecontentnew += "<div class=lista-comentarios>";
    for (size_t i=0; i<pages.size(); ++i)
    {
        pagecontentnew += pages[i].comments;
    }
    pagecontentnew += pagecontentold.substr(pagecontentold.find("<div class=\"light-pagination"));

    string pageunselected = "class=\"option number\"";
    string pageselected = "class=\"option number active\"";
    contentFindReplaceAll(pagecontentnew, pageunselected, pageselected);

    string filereplace = "(*" + to_string(pages.size()) + ") .html";
    string routenew = routeold.replace(routeold.find(".html"), sizeof(".html") - 1, filereplace);
    ofstream thefilenew(routenew);
    thefilenew << pagecontentnew;

    int pagecomments = countSubstring(pagecontentnew, ">ANON</div> ");
    cout << "##############################\nLas copias fueron combinadas correctamente, " << pages.size() << " páginas de " << pagecomments << " comentarios en total ahora se encuentran en un mismo archivo. \n##############################" << endl;
    return 0;
}

string contentBetween(string full, string del1, string del2){
    size_t sc = full.find(del1);
    size_t ec = full.find(del2, sc);
    string content = full.substr(sc + del1.length(), ec - sc - 1 - del2.length());
    return content;
}

string contentBetweenUntil(string full, string del1){
    string line;
    istringstream stream(full);
    string find = "no";
    while (find == "no" && getline(stream, line)) {
        if (line.find(del1) != string::npos) {
            find = line.substr(line.find(del1) + del1.size()); 
            break;
        }
    }
    return find;
}

pageschema fileRead(string file)
{
    ifstream inf (file.c_str());
    if (!inf)
    {
        cerr << "##############################\nError! Uno de los archivos no pudo ser analizado. \n##############################" << endl;
        exit(1);
    } 
    string pagecontent( (istreambuf_iterator<char>(inf) ),
                       (istreambuf_iterator<char>()    ) );

    int pagesize = pagecontent.length();
    string sc1 = "<div class=lista-comentarios>";
    string sc2 = "<div class=\"light-pagination";
    if (!(pagecontent.find(sc2) != string::npos)) {
        cerr << "##############################\nError! Uno de los archivos no es válido. \n##############################" << endl;
        exit(1);
    } 
    string pagecomments = contentBetween(pagecontent, sc1, sc2);
    string advanced = "role=presentation";
    string sn1 = " active\" role=presentation><span>";
    if (!(pagecontent.find(advanced) != string::npos)) {
        string sn1 = " active\"><span>";
    } 
    string pagenumbers = pagecontent.substr(pagecontent.find(sn1) + sn1.length()); 
    pagenumbers = pagenumbers[0];
    int pagenumber = stoi(pagenumbers);

    string su1 = "url: ";
    string su2 = "saved date: ";
    string pageurl = contentBetween(pagecontent, su1, su2);

    // string sd1 = "saved date: ";
    // string pagedate = contentBetweenUntil(pagecontent, sd1);

    pageschema thepage;
    thepage.number = pagenumber; 
    thepage.comments = pagecomments; 
    thepage.size = pagesize;
    thepage.route = file;
    thepage.url = pageurl;
    // thepage.date = pagedate;
    return thepage;
}

vector<string> fileRoutes(string routesstring){
    vector<string> routesvector;
    string token;
    stringstream ss(routesstring);
    while (getline(ss, token, '\'')){
        if (token.length() > 3) {
            routesvector.push_back(token);
        }
    }
    return routesvector;
}

int main(int argc, char* argv[]){
    vector<string> routesvector(argv + 1, argv + argc);
    if (routesvector.size() < 2){
        string routesstring;
        cout << "##############################\nIntroduce la ruta de cada archivo a combinar: \n##############################" << endl;
        getline(cin, routesstring);
        routesvector = fileRoutes(routesstring);
        if (routesvector.size() < 2){
            routesvector.clear();
            int routesnumber;
            cout << "##############################\nSe requieren al menos dos rutas. Introduce la cantidad de archivos a combinar, en número: \n##############################" << endl;
            cin >> routesnumber;
            cin.ignore();
            while (routesvector.size() < routesnumber)
            {
                if (routesvector.size() == 0) {
                    cout << "##############################\nIntroduce la primera ruta: \n##############################" << endl;
                }
                else {
                    cout << "##############################\nIntroduce la siguiente ruta: \n##############################" << endl;
                }
                getline(cin, routesstring);
                routesstring = routesstring.substr(1, routesstring.size() - 2);
                routesvector.push_back(routesstring);
            }
        }
    }
    vector<pageschema> pageobjects;
    for (size_t i=0; i<routesvector.size(); ++i)
    {
        pageschema thepage = fileRead(routesvector[i]);
		pageobjects.push_back(thepage);
    }
    sort( pageobjects.begin( ), pageobjects.end( ), [ ]( const auto& lhs, const auto& rhs )
    {
        return lhs.number < rhs.number;
    });
    string pageselectedroute;
    int pageselectedsize = 0;
    string pageselectedurl;
    for (size_t i=0; i<pageobjects.size(); ++i)
    {
        if (pageselectedsize < pageobjects[i].size) {
            pageselectedsize = pageobjects[i].size;
            pageselectedroute = pageobjects[i].route;
        }
        if (i> 0 & pageselectedurl != pageobjects[i].url) {
            cerr << "##############################\nError! Uno de los archivos no coincidió en dirección con otro.\n##############################" << endl;
            exit(1);
        }
        else{
            pageselectedurl = pageobjects[i].url;
        }
    }
    fileServe(pageselectedroute, pageobjects);
    return 0;
}