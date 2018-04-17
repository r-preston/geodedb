#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <cmath>

#include "headers/structures.h"
#include "headers/stonebase.h"

std::string byte_to_string(int i)
{
    return std::to_string(i);
}

std::string byte_to_string(bool b)
{
    if(b) return "1";
    return "0";
}

void reset_object(mineral_entry & data)
{
    data.id = -1;
    data.pagenumber = -1;
    data.category = "";
    data.name = "";
    data.description = "";
    data.structure = "";
    data.composition = "";
    data.varieties.clear();
    data.colours.clear();
    data.images.clear();
}

void reset_object(collection_entry & data)
{
    data.id = -1;
    data.purchase = -1;
    data.cost = -1;
    data.texture = -1;
    data.order = "";
    data.name = "";
    data.description = "";
    data.date = "";
    data.buyer = "";
    data.notes = "";
    data.buy_location = "";
    data.now_location = "";
    data.mineral.clear();
    data.images.clear();
    data.colours.clear();
}

std::string namepart(std::string filepath)
{
    size_t t = filepath.find_last_of('/');
    return filepath.substr(t+1);
}

const char * plural_correction(const int i)
{
    if(i == 1) return "";
    return "s";
}

int censor(std::string &word)
{
    /*
    censor removes every character in a string that is not a letter of the alphabet
    this is used on table names
    if you type in 'ch335ec@k3' for a table name, it will automatically be converted to 'check'
    */
    std::string valid_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string newword = "";

    for(auto c:word)
    {
        // iterate through each letter of word
        if(valid_chars.find(c) != std::string::npos)
        {
            // if c is in valid_chars, add to newword
            newword += c;
        }
    }
    if(newword.size() == 0) // if none of the characters in word were valid, throw an error (0)
        return 1;
    word = newword; // otherwise, set word to newword
    return 0;
}

std::string prune(std::string s)
{
    if(s.size() > 32)
        return (s.substr(0,32)+"...");
    return s;
}

bool to_bool(const std::string s)
{
     return s != "0";
}

void eliminate_whitespace(std::string &str)
{
    /*
    specifically eliminates whitespace which does not lie between and '=' and a semicolon in a string
    used when adding items from a file
    */

    std::string s;
    bool eliminate = true; // start out by removing whitespace

    for(auto c:str) // for each character
    {
        if(c == '=') // stop removing whitespace
            eliminate = false;
        else if(c == ';') // start removing whitespace again
            eliminate = true;

        if(!(c == ' ' && eliminate))
            s += c; // if char is not whitespace, add to buffer string
    }

    str = s;
}

bool does_file_exist(const std::string name)
{
    // checks if a file exists
    std::ifstream f(name.c_str());
    return f.good();
}

bool portable_condition()
{
    return does_file_exist("portable");
}

std::vector<std::string> explode(std::string s, char c)
{
    /*
    splits a string on c
    for example, explode("list,of,words",',') returns {"list","of","words"}
    function ignores any instances of c inside double quotes
    */

    std::string buffer; // stores current word
    std::vector<std::string> v; // output
    bool track = true; // keeps track of whether the loop is in a double quote pair or not

    for(auto n:s) // iterate through s
    {
        if(n == '"')
            track = !track; // we have just entered/left a quote pair
        if(!track)
        {
            buffer += n; // if in quote pair, add n to buffer regardless
            continue; // skip any further comparisons
        }
        if(n != c)
            buffer += n; // if n is not c, add to buffer
        else if((n == c) && (buffer != "") && track)
        {
            v.push_back(buffer); // if n is c, add buffer to output
            buffer = ""; // reset buffer
        }
    }
    if(buffer !=  "") // if there is something in the buffer
        v.push_back(buffer); // add whatever buffer is to output

    return v;
}

bool is_integer(const std::string s, bool allow_void = true)
{
    /*
    checks if s contains only numbers, i.e. s is an integer
    by default, '~' is accepted, which is translated as NULL
    */
    if(s == "~" && allow_void) return true;
    std::string digits = "0123456789";
    for(auto n:s) // iterate through s
        if(digits.find(n) == std::string::npos) // if n is not a number
            return false;
    return true; // s is a number
}

bool is_antinumber(const std::string s)
{
    /*
    opposite of is_integer - returns false if s contains any numbers
    */
    std::string digits = "0123456789";
    for(auto n:s) // iterate through the string
        if(digits.find(n) != std::string::npos) // n is a number! D:
            return false;
    return true; // no number here
}

bool is_binary(const std::string s)
{
    /*
    returns true is s contains only binary digits
    also accepts ~ as NULL
    */
    if(s == "~") return true;
    std::string digits = "01";
    for(auto n:s) // iterate through string
        if(digits.find(n) == std::string::npos) // if n is not 1 or 0
            return false;
    return true;
}

std::string vec_to_str(std::vector<std::string> v, bool returntilde = false)
{
    /*
    turns a vector of strings into a single string composed of the values in v separated by commas
    if returntilde is true, the function returns a tilde when the list is empty. This is used when searching SQLite database
    */
    if(!v.size())
    {
        return (returntilde ? "~" : ""); // nothing given, see as NULL
    }
    std::string output = "";
    for(unsigned int i = 0; i < v.size()-1; i++)
    {
        // for each item except the last on, add to output and follow with a comma
        output += v[i];
        output += ",";
    }
    // add in the final value. number of commas should be n-1 for a vector with n items
    output += v[v.size()-1];
    return output;
}

std::string vec_to_sepstring(std::vector<std::string> v, bool returntilde = false)
{
    /*
    turns a vector of strings into a single string composed of the values in v separated by commas
    if returntilde is true, the function returns a tilde when the list is empty. This is used when searching SQLite database
    */
    if(!v.size())
    {
        return (returntilde ? "~" : ""); // nothing given, see as NULL
    }
    std::string output = "";
    for(unsigned int i = 0; i < v.size()-1; i++)
    {
        // for each item except the last on, add to output and follow with a comma
        output += v[i];
        output += '\u001c';
    }
    // add in the final value. number of commas should be n-1 for a vector with n items
    output += v[v.size()-1];
    return output;
}

std::string shrt_long_date(std::string shortdate)
{
    /*
    converts dd-mm-yyyy dates to DD Month yyyy
    e.g. 01-03-1991 -> 1st March 1991
    */
    std::string months[12] = {"January","February","March","April","May","June","July","August","September","October","November","December"};
    std::string longdate = "";
    if(shortdate[0] == '0') // if the first digit is 0, the day is less than 10, so only ass the last digit
        longdate += shortdate[1];
    else
        longdate += shortdate.substr(0,2); // otherwise the day has two digits, add both
    if(longdate[longdate.size()-1] == '1') // if the day is a first, add st
        longdate += "st ";
    else if(longdate[longdate.size()-1] == '2') // nd for something second dates
        longdate += "nd ";
    else if(longdate[longdate.size()-1] == '3') // rd for 3
        longdate += "rd ";
    else // otherwise just add th
        longdate += "th ";
    // note that each nth suffix is followed by a space
    // get the (mm-1)th item of months - which is the corresponding month
    longdate += months[std::stoi(shortdate.substr(3,2))-1];
    // add space
    longdate += " ";
    // add in the date, no modification neeeded
    longdate += shortdate.substr(6,4);
    return longdate;
}

std::string vector_int_to_string(std::vector<int> v)
{
    /*
    turns a vector of ints into a single string composed of the values in v separated by commas
    */

    if(!v.size()) return ""; // nothing given, see as NULL
    std::string output = "";
    for(unsigned int i = 0; i < v.size()-1; i++)
    {
        // for each item except the last on, add to output and follow with a comma
        output += std::to_string(v[i]);
        output += ",";
    }
    // add in the final value. number of commas should be n-1 for a vector with n items
    output += std::to_string(v[v.size()-1]);
    return output;
}

std::vector<int> string_to_vector_int(std::string s)
{
    /*
    turns a comma-separated string such as might be produced by vector_int_to_string and turns it back into a vector
    */
    std::vector<int> result;
    std::vector<std::string> v = explode(s, ',');
    for(std::string cl:v)
    {
        result.push_back(std::stoi(cl));
    }
    return result;
}

std::ifstream::streampos filesize(const char* filename)
{
    std::ifstream in(filename, std::ios::ate | std::ios::binary);
    return in.tellg(); 
}

std::string pence_to_pounds(int pence)
{
    /*
    formats a number of pennies into readable money type
    if pence > 99 convert pence into pounds, with a � on the front
    otherwise, leave as pence with a p on the back
    */
    std::string strpounds; // stores output

    if(pence < 100)
    {
        strpounds = std::to_string(pence);
        strpounds += "p";
        return strpounds;
    }
    strpounds = "\x9C"; // pound sign
    double pounds = double(pence)/100; // divide by 100, keeping decimal place to turn pence into an amount in pounds
    strpounds += std::to_string(pounds); // convert double value to string

    int beforepoint = (int)std::floor(log10(pounds)); // works out the floored log of the number, which is equal to the number of figures before the decimal point minus 1
    if(beforepoint < 0) beforepoint = 0; // 0 means that the number is < 10

    // crop the string to 2 decimal places
    // reason for the +5:
    //   beforepoint is equal to number of figures before the decimal point minus 1, so add 1 to make up for that
    //   another 1 is added for the pound sign on the front
    //   1 is added because of the decimal point
    //   2 is added for the 2 decimal places
    strpounds = strpounds.substr(0, beforepoint + 5);

    return strpounds;
}

std::string rough_or_tumbled(int texture, bool capital = false)
{
    /*
    prints out what collection_entry::texture means
    capital specifies whether the result should be capitalised or not
    if texture is 1, the stone is tumbled
    if texture is 0, the stone is rough
    if capital is true, capitalise
    */

    if(texture && capital)
        return "Tumbled";
    else if(texture)
        return "tumbled";
    else if(!texture && capital)
        return "Rough";
    return "rough";
}

std::string visualise_vector(std::vector<std::string> v)
{
    /*
    similar to vec_to_str(), but grammatically and aesthetically nicer
    example:
        visualise_vector( {"a","vector","of","words"} )
    gives
        "a, vector, of and words"

    in contrast, vec_to_str( {"a","vector","of","words"} )
    gives "a,vector,of,words"
    */

    std::string result = ""; // stores output

    if(v.size() == 1)
    {
        // no commas required
        result = v[0];
    }
    else if (v.size() == 0)
    {
        result = "";
    }
    else
    {
        // for each item bar the last two, print and separate by commas
        for(unsigned int i = 0; i < v.size()-2; i++)
        {
            result += v[i];
            result += ", ";
        }
        // the last two are separated by and, because nice grammar :)
        result += v[v.size()-2];
        result += " and ";
        result += v[v.size()-1];
    }
    return result;
}

void sanitise_path(std::string &path)
{
    /*
    Both ShellExecute for windows and Unix-like systems prefer a forward slash (/) instead of a backslash (\) as a separator in file paths
    this function replaces all forward slashes in a string with backslashes
    */

    std::string adjusted_path; // stores new path

    for(auto c:path) // for every character in the given path
    {
        if(c == '\\') // if that character is a forward slash
            adjusted_path += "/"; // add a backslash to the new path instead (\\ is the control character for backslash in an std::string or char *)
        else
            adjusted_path += c; // otherwise add character to new path
    }

    path = adjusted_path; // set path to new path
}

void example_file(bool collection)
{
    /*
    creates an file containing an example of the formatting understood by add_to_file
    if collection is true, syntax is for collection-type tables
    if collection is false, syntax is for the Minerals table
    */

    std::ofstream writefile;

    if(collection)
    {
        writefile.open("example_collection.txt", std::fstream::in | std::fstream::trunc);

        writefile << "% any line starting with a '%' is treated as a comment and is ignored\n";
        writefile << "% each row goes on its own line - use '\\n' to indicate line breaks in a field\n";
        writefile << "% rows can appear in any order, and do not all have to be provided\n";


        writefile << "{\n";
        writefile << "% Purchase number\n";
        writefile << "num=1\n";
        writefile << "% Order in purchase\n";
        writefile << "order=a\n";
        writefile << "% Name\n";
        writefile << "name=example name\n";
        writefile << "% Minerals making up the stone\n";
        writefile << "type=mineral type 1,mineral type 2\n";
        writefile << "% Description\n";
        writefile << "desc=description\n";
        writefile << "% List of colours present in the stone\n";
        writefile << "colours=red,orange,yellow\n";
        writefile << "% Date must be of the form dd-mm-yyyy\n";
        writefile << "date=02-02-1999\n";
        writefile << "% Location of purchase\n";
        writefile << "from=location of purchase\n";
        writefile << "% Notes on the stone\n";
        writefile << "notes=some notes\n";
        writefile << "% Cost must always be in pence\n";
        writefile << "cost=10\n";
        writefile << "% Person who bought the stone\n";
        writefile << "buyer=someone\n";
        writefile << "% Current location of the stone\n";
        writefile << "location=somewhere\n";
        writefile << "% Texture is 1 if tumbled, 0 if rough\n";
        writefile << "texture=1\n";
        writefile << "% Image paths\n";
        writefile << "images=an\\image\\path.jpg,another\\image\\path.png\n";
        writefile << "}\n";

        writefile << "{\n";
        writefile << "num=2\n";
        writefile << "order=a\n";
        writefile << "name=a different example\n";
        writefile << "images=more\\image\\paths.jpg\n";
        writefile << "}";
    }
    else
    {
        writefile.open("example_minerals.txt", std::fstream::in | std::fstream::trunc);

        writefile << "% any line starting with a '%' is treated as a comment and is ignored\n";
        writefile << "% each row goes on its own line - use '\\n' to indicate line breaks in a field\n";
        writefile << "% rows can appear in any order, and do not all have to be provided\n";

        writefile << "{\n";
        writefile << "% name of the mineral\n";
        writefile << "name=example name\n";
        writefile << "% page number in 'The Complete Encyclopaedia of Minerals'\n";
        writefile << "pagenum=122\n";
        writefile << "% Nickel-Strunz mineral category as used by the above book\n";
        writefile << "category=oxide\n";
        writefile << "% Varieties of the mineral\n";
        writefile << "varieties=variety 1,variety 2\n";
        writefile << "% Crystal structure\n";
        writefile << "structure=monoclinic\n";
        writefile << "% Chemical formula\n";
        writefile << "formula=CaCO3\n";
        writefile << "% Common colours\n";
        writefile << "colours=green,blue,indigo,violet\n";
        writefile << "% Description\n";
        writefile << "desc=description\n";
        writefile << "% Image paths\n";
        writefile << "images=image\\path.jpg,another\\path.jpg\n";
        writefile << "}\n";

        writefile << "{\n";
        writefile << "name=example name\n";
        writefile << "varieties=variety 1,variety 2\n";
        writefile << "}\n";
    }

    writefile.close();

    //valid_names = {"name", "pagenum", "category", "varieties", "structure", "formula", "colours", "desc", "images"};
}

bool is_valid_date(std::string date)
{
    // checks if a string is in the form dd-mm-yyyy
    if(date.size() != 10)
        return false;
    if(!is_integer(date.substr(0,2)))
        return false;
    if(!is_integer(date.substr(3,2)))
        return false;
    if(!is_integer(date.substr(6,4)))
        return false;
    if(date[2] != '-')
        return false;
    if(date[5] != '-')
        return false;
    return true;
}

void escape_string(std::string &s)
{
    /*
    this function converts the string value "\\n" into the carriage return character '\n' in the given string
    this allows users to insert line breaks into text without needing to press enter
    */

    size_t pos = s.find("\\n"); // find the target string

    while(pos != std::string::npos) // while an instance of the target string exists
    {
        s.replace(pos, 2, "\n"); // replace it with the line feed character
        pos = s.find("\\n"); // look for target again
    }
}

void nullify_collection_fields(collection_entry &centr)
{
    /*
    change all empty std::string fields in a collection_entry to '~'
    this ensures proper treatment when adding new row
    */

    if(centr.order == "")
        centr.order = "~";
    if(centr.name == "")
        centr.name = "~";
    if(centr.description == "")
        centr.description = "~";
    if(centr.date == "")
        centr.date = "~";
    if(centr.buyer == "")
        centr.buyer = "~";
    if(centr.notes == "")
        centr.notes = "~";
    if(centr.buy_location == "")
        centr.buy_location = "~";
    if(centr.now_location == "")
        centr.now_location = "~";
}

void nullify_mineral_fields(mineral_entry &mentr)
{
    /*
    change all empty std::string fields in a mineral_entry to '~'
    this ensures proper treatment when adding new row
    */

    if(mentr.name == "")
        mentr.name = "~";
    if(mentr.structure == "")
        mentr.structure = "~";
    if(mentr.description == "")
        mentr.description = "~";
    if(mentr.category == "")
        mentr.category = "~";
    if(mentr.composition == "")
        mentr.composition = "~";
}

std::string s(int i)
{
    return std::to_string(i);
}

void interpret_excel_output(std::string & line)
{
    size_t remove_pos = 0;
    size_t comma_position = line.find(',');
    size_t last_quote_pos = line.find_last_of('"');

    last_quote_pos = (last_quote_pos == std::string::npos) ? 0 : last_quote_pos;
    last_quote_pos = (last_quote_pos < comma_position) ? comma_position : last_quote_pos;

    remove_pos = line.find(',', last_quote_pos + 1);
    while (remove_pos != std::string::npos)
    {
        line.erase(remove_pos, 1);
        remove_pos = line.find(',', last_quote_pos + 1);
    }
    if (line.size() > (comma_position + 2))
    {
        if (line[comma_position + 1] == '"')
        {
            line.erase(comma_position + 1, 1);
            line.erase(line.size() - 1, 1);

            unsigned int quote_streak = 0;
            for (unsigned int i = comma_position; i < line.size(); i++)
            {
                if(line[i] == '"') quote_streak++;
                else if(quote_streak > 0)
                {
                    if (quote_streak % 2 == 0)
                    {
                        line.erase(i - (quote_streak / 2), quote_streak / 2);
                        i -= quote_streak / 2;
                    }
                    quote_streak = 0;
                }
            }
            if(quote_streak > 0)
            {
                if (quote_streak % 2 == 0)
                {
                    line.erase(line.size() - 1 - (quote_streak / 2), quote_streak / 2);
                }
            }
        }
    }
}

bool fuzzy_date_correction(std::string & date)
{
    if((date.find("-") == std::string::npos) && (date.find("/") == std::string::npos)) return false;
    while (date.find('/') != std::string::npos)
    {
        date.replace(date.find('/'), 1, "-");
    }
    std::vector<std::string> date_parts = explode(date, '-');

    if(date_parts.size() != 3) return false;
    if(date_parts[0].size() > 2) return false;
    if(date_parts[0].size() < 1) return false;
    if(date_parts[1].size() > 2) return false;
    if(date_parts[1].size() < 1) return false;
    if(date_parts[2].size() != 4) return false;

    if(!is_integer(date_parts[0], false)) return false;
    if(!is_integer(date_parts[1], false)) return false;
    if(!is_integer(date_parts[2], false)) return false;

    if(date_parts[0].size() == 1) { date_parts[0] = "0" + date_parts[0]; }
    if(date_parts[1].size() == 1) { date_parts[1] = "0" + date_parts[1]; }

    date = date_parts[0] + "-" + date_parts[1] + "-" + date_parts[2];
    if(date.size() != 10) return false;

    return true;
}

bool fuzzy_money_correction(std::string & moneystring)
{
    if(is_integer(moneystring, false)) return true;
    if ((moneystring[0] == '£') || (moneystring[0] == '\xA3') || (moneystring[0] == '\x9C')) // don't talk to me about Windows' character encoding
    {
        moneystring.erase(moneystring.begin());
        std::vector<std::string> moneyparts = explode(moneystring, '.');
        for (auto part : moneyparts)
        {
            if(!is_integer(part, false)) return false;
        }
        if(moneystring.size() == 0) return false;
        double pence_value = std::stod(moneystring) * 100;
        if(floor(pence_value) != pence_value) return false;
        moneystring = s( (int)floor(pence_value) );
        return true;
    }
    else if (moneystring.back() == 'p')
    {
        moneystring.pop_back();
        if(!is_integer(moneystring)) return false;
        return true;
    }
    return false;
}

bool fuzzy_texture_correction(std::string & texture)
{
    std::transform(texture.begin(), texture.end(), texture.begin(), ::tolower);
    if (texture == "rough")
    {
        texture = "0";
        return true;
    }
    if (texture == "tumbled")
    {
        texture = "1";
        return true;
    }
    if (texture == "raw")
    {
        texture = "0";
        return true;
    }
    if (texture == "smooth")
    {
        texture = "1";
        return true;
    }
    if (texture == "1")
    {
        return true;
    }
    if (texture == "0")
    {
        return true;
    }
    if (texture == "-1")
    {
        return true;
    }
    return false;
}

bool check_csv_line(std::string & data, const int verification)
{
    if (verification == 0)
    {
        if(data == "") data = "~";
    }
    else if (verification == 1)
    {
        if(data == "")
        {
            data = "-1";
            return true;
        }
        if(!is_integer(data, false)) return false;
    }
    else if (verification == 2)
    {
        if(data == "")
        {
            data = "~";
            return true;
        }
        if(!fuzzy_date_correction(data)) return false;
    }
    else if (verification == 3)
    {
        if(data == "")
        {
            data = "~";
            return true;
        }
        if(!is_antinumber(data)) return false;
    }
    else if (verification == 4)
    {
        if(data == "")
        {
            data = "-1";
            return true;
        }
        if(!fuzzy_money_correction(data)) return false;
    }
    else if (verification == 5)
    {
        if(data == "")
        {
            data = "-1";
            return true;
        }
        if(!fuzzy_texture_correction(data)) return false;
    }
    return true;
}

std::vector<std::string> parse_input_file(std::string filename, std::string tablename, int errorfree = 0)
{
    std::vector<std::string> errors, identifiers, idbuffer, databuffer, overwrite_abbreviations, overwrite;
    std::vector<int> verification;
    int linenum = 0;
    unsigned int entry_count;
    int rowid;
    std::string line;

    std::ifstream readfile(filename);

    if (tablename == "Minerals")
    {
        overwrite_abbreviations = {"name", "pagenum", "category", "structure", "formula", "desc", "images", "varieties", "colours"};
        // 0 = none required, 1 = integer, 2 = date, 3 = contains no numbers, 4 = money, 5 = texture, 6 = nothing, but don't return tilde
        identifiers = {"Name", "Varieties", "Description", "Colours", "Crystal structure", "Chemical formula", "Nickel-Strunz category", "Page number"};
        verification = {0, 6, 0, 6, 0, 0, 0, 1};
        entry_count = 8;
    }
    else
    {
        overwrite_abbreviations = {"order", "name", "desc", "date", "from", "notes", "buyer", "location", "num", "cost", "texture", "images", "type", "colours"};
        // 0 = none required, 1 = integer, 2 = date, 3 = contains no numbers, 4 = money, 5 = texture, 6 = nothing, but don't return tilde
        identifiers = {"Purchase number", "Order in purchase", "Name", "Description", "Mineral types", "Colours", "Notes", 
                       "Buyer", "Cost", "Date of purchase", "Purchase location", "Current location", "Texture"};
        verification = {1, 3, 0, 0, 6, 6, 0, 0, 4, 2, 0, 0, 5};
        entry_count = 13;
    }

    while (getline(readfile, line))
    {
        linenum++;
        if (line.size() == 0)
            continue;
        if(line[0] == '#')
            continue;
        if(line == ",")
            continue;
        if (line.find(',') == std::string::npos)
        {
            errors.push_back("Line "+s(linenum)+": no comma found, are you sure you exported as a .CSV file?");
            continue;
        }
        interpret_excel_output(line);
        
        if (idbuffer.size() < entry_count)
        {
            idbuffer.push_back(line.substr(0, line.find(',')));
            databuffer.push_back(line.substr(line.find(',') + 1));
        }
        if (idbuffer.size() == entry_count)
        {
            for (unsigned int i = 0; i < idbuffer.size(); i++)
            {
                if (std::find(identifiers.begin(), identifiers.end(), idbuffer[i]) == identifiers.end())
                {
                    errors.push_back("Line "+s(linenum - idbuffer.size() + i)+": unknown row identifier '"+ idbuffer[i] +"'");
                }
                else if (std::find(identifiers.begin() + i + 1, identifiers.end(), idbuffer[i]) != identifiers.end())
                {
                    errors.push_back("Line "+s(linenum - idbuffer.size() + 
                                     (std::find(identifiers.begin() + i + 1, identifiers.end(), idbuffer[i])) - identifiers.begin() ) + 
                                     ": row identifier '"+ idbuffer[i] +"' is duplicated");
                }

                if (verification[i] == 0)
                {
                    if(!check_csv_line(databuffer[i], 0))
                        errors.push_back("Line "+s(linenum - idbuffer.size() + i)+": the program doesn't like the value "+databuffer[i]+", I don't know how you managed this");
                }
                else if (verification[i] == 1)
                {
                    if(!check_csv_line(databuffer[i], 1))
                        errors.push_back("Line "+s(linenum - idbuffer.size() + i)+": value given is not an integer");
                }
                else if (verification[i] == 2)
                {
                    if(!check_csv_line(databuffer[i], 2))
                        errors.push_back("Line "+s(linenum - idbuffer.size() + i)+": value is not a valid date - it should be in the form dd-mm-yyyy");
                }
                else if (verification[i] == 3)
                {
                    if(!check_csv_line(databuffer[i], 3))
                        errors.push_back("Line "+s(linenum - idbuffer.size() + i)+": value should not contain numbers");
                }
                else if (verification[i] == 4)
                {
                    if(!check_csv_line(databuffer[i], 4))
                        errors.push_back("Line "+s(linenum - idbuffer.size() + i)+": value is not recognisable as an amount of money");
                }
                else if (verification[i] == 5)
                {
                    if(!check_csv_line(databuffer[i], 5))
                        errors.push_back("Line "+s(linenum - idbuffer.size() + i)+": texture has two possible values - Rough or Tumbled (case insensitive)");
                }
            }
            if (errorfree)
            {
                // add the item
                if (tablename == "Minerals")
                {
                    mineral_entry mentry;
                    for (unsigned int i = 0; i < idbuffer.size(); i++)
                    {
                        if(idbuffer[i] == "Name")                        mentry.name = std::stoi(databuffer[i]);
                        else if(idbuffer[i] == "Varieties")              mentry.varieties = explode(databuffer[i], ',');
                        else if(idbuffer[i] == "Description")            mentry.description = databuffer[i];
                        else if(idbuffer[i] == "Colours")                mentry.colours = explode(databuffer[i], ',');
                        else if(idbuffer[i] == "Crystal structure")      mentry.structure = databuffer[i];
                        else if(idbuffer[i] == "Chemical formula")       mentry.composition = databuffer[i];
                        else if(idbuffer[i] == "Nickel-Strunz category") mentry.category = databuffer[i];
                        else if(idbuffer[i] == "Page number")            mentry.pagenumber = std::stoi(databuffer[i]);
                    }

                    does_exist("Minerals", -1, mentry.name, rowid);

                    if ((rowid != -1) && (errorfree == 2)) // the item about to be added is a duplicate
                    {
                        mentry.id = rowid;
                        const char * errmsg = update_mineral_item(mentry, mentry, overwrite_abbreviations, "");
                        if(errmsg) errors.push_back("fail");
                        else errors.push_back("success");
                    }
                    else if (errorfree != 1)
                    {
                        const char * errmsg = new_mineral_row(mentry, "");
                        if(errmsg) errors.push_back("fail");
                        else errors.push_back("success");
                    }
                }
                else
                {
                    collection_entry centry;
                    for (unsigned int i = 0; i < idbuffer.size(); i++)
                    {
                        if(idbuffer[i] == "Purchase number")        centry.purchase = std::stoi(databuffer[i]);
                        else if(idbuffer[i] == "Order in purchase") centry.order = databuffer[i];
                        else if(idbuffer[i] == "Name")              centry.name = databuffer[i];
                        else if(idbuffer[i] == "Description")       centry.description = databuffer[i];
                        else if(idbuffer[i] == "Mineral types")     centry.mineral = explode(databuffer[i], ',');
                        else if(idbuffer[i] == "Colours")           centry.colours = explode(databuffer[i], ',');
                        else if(idbuffer[i] == "Notes")             centry.notes = databuffer[i];
                        else if(idbuffer[i] == "Buyer")             centry.buyer = databuffer[i];
                        else if(idbuffer[i] == "Cost")              centry.cost = std::stoi(databuffer[i]);
                        else if(idbuffer[i] == "Date of purchase")  centry.date = databuffer[i];
                        else if(idbuffer[i] == "Purchase location") centry.buy_location = databuffer[i];
                        else if(idbuffer[i] == "Current location")  centry.now_location = databuffer[i];
                        else if(idbuffer[i] == "Texture")           centry.texture = std::stoi(databuffer[i]);
                    }

                    does_exist(tablename, centry.purchase, centry.order, rowid);

                    if ((rowid != -1) && (errorfree == 2)) // the item about to be added is a duplicate
                    {
                        centry.id = rowid;
                        const char * errmsg = update_collection_item(centry, centry, tablename, overwrite_abbreviations, "");
                        if(errmsg) errors.push_back("fail");
                        else errors.push_back("success");
                    }
                    else if (errorfree != 1)
                    {
                        for (int j = 0; j < 1000; j++)
                        {
                            const char * errmsg = new_collection_row(centry, tablename, "");
                            if(errmsg) errors.push_back("fail");
                            else errors.push_back("success");
                        }
                    }
                }
            }
            idbuffer.clear();
            databuffer.clear();
        }
    }

    if ((idbuffer.size() != entry_count) && (idbuffer.size() > 0))
    {
        errors.push_back("Line "+s(linenum)+": missing row in this entry");
    }

    return errors;
}
