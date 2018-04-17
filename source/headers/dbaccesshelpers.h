#ifndef DATABASE_ACCESS_HELPER_FUNCTIONS
#define DATABASE_ACCESS_HELPER_FUNCTIONS

#include "parsing.h"

// a filthy hack for populating a pointer to a vector with strings via a void*
// see table_callback()
typedef std::vector<std::string> table_type;

std::string assimilate_image(std::string image_path, std::string path)
{
    std::string filename = namepart(path);
    int i = 1;
    bool exists = false;
    size_t ext_pos;
    std::string extension;

    while (does_file_exist(image_path+filename+extension))
    {
        if(!exists)
        {
            ext_pos = filename.find_last_of('.');
            extension = filename.substr(ext_pos);
            filename = filename.substr(0, ext_pos);
            filename += "_1";
        }
        else
        {
            while (filename.back() != '_')
            {
                filename.pop_back();
            }
            filename += std::to_string(i);
        }
        i++;
        exists = true;
    }
    if(exists) filename += extension;

    std::ifstream src(path, std::ios::binary);
    std::ofstream dst(image_path+filename, std::ios::binary);
    dst << src.rdbuf();

    return filename;
}

bool datestrcomp(std::string a, std::string b, int type)
{
    /*
    this function compares two numeric strings to find out which has the higher/lower value
    type specifies comparison type = -1 is less than, 0 is equals, 1 is greater than
    the strings could be XX or XXXX, which means they could stand for any number
    */

    if(a == "XX" || a == "XXXX")
        return true;
    if (type < 0)
        return a < b; // true if a<b, otherwise false
    else if(type > 0)
        return a > b; // true if a>b, else false
    else
        return a == b; // true if a=b, else false
}

bool datecmp(std::string query, std::string model, int type = -1)
{
    /*
    this function compares two date strings of form dd-mm-yyyy (query is compared to model)
    any part of the date could be XX or XXXX, meaning that that part of the date could stand for any value
    e.g. XX-XX-2016 == 02-04-2016 is true, and XX-XX-1990 > 02-02-1990 is false

    this function works by comparing the year, then month if year is not decisive, then day

    if type is 0, equality is checked
    if type is -1, query < model is checked
    if type is 1, query > model is checked

    indexes of date parts:
    dd-mm-yyyy
    01-34-6789
    */

    if(query == "") return 0;
    if(model == "") return 0;

    // set each part of the date to a separate string for ease of comparison
    std::string qday = query.substr(0,2);
    std::string mday = model.substr(0,2);
    std::string qmonth = query.substr(3,2);
    std::string mmonth = model.substr(3,2);
    std::string qyear = query.substr(6,4);
    std::string myear = model.substr(6,4);

    if(type == 0 && datestrcomp(qday,mday,0) && datestrcomp(qmonth,mmonth,0) && datestrcomp(qyear,myear,0))
    {
        // if each part of the dates are equal, return true
        // as datestrcmp returns true for XX=anything, if any part of the date is XX then it is automatically treated as true
        return true;
    }
    else if(type == 0)
    {
        // if any part of the date does not match, dates are not the same
        return false;
    }

    if( datestrcomp(qyear,myear,type) )
    {
        if( datestrcomp(qmonth,mmonth,type) )
        {
            if( datestrcomp(qday,mday,type) ) return true;
        }
    }  
    /*if( datestrcomp(qyear,myear,type) )
    {
        return true; // if one year if different to the other, the month and day are irrelevant
    }
    else if(!datestrcomp(qyear,myear,type*-1))
    {
        // if years are the same, check the month
        if( datestrcomp(qmonth,mmonth,type) )
            return true; // if months are different, day is irrelevant
        else if(!datestrcomp(qmonth,mmonth,type*-1))
        {
            // if the months are the same, check the day
            // this will decide if the dates are < or > to each other
            if( datestrcomp(qday,mday,type) ) return true;
        }
    }*/

    // if none of the above conditions are matched, the dates no not satisfy the condition
    return false;
}

int empty_callback(void *data, int columncount, char **fields, char **columns)
{
    /*
    does absolutely nothing.
    used with sqlite_exec where there will be no output
    */
    return 0;
}

int table_callback(void *data, int columncount, char **fields, char **columns)
{
    /*
    populates a vector with results from SELECT, for use with sqlite3_exec
    I didn't know how to use sqlite3_step properly then, and I can't be bothered to upgrade those functions
    if it ain't broke, don't fix it
    */

    // cast data to a table_type (which is an std::vector<std::string> in disguise)
    // *table now gains the properties of an std::vector
    table_type* table = static_cast<table_type*>(data);
    // implicitly convert the name (char*) to std::string
    std::string nm = fields[0];
    // push it into table
    table->push_back(nm);
    // run away before anyone notices
    return 0;
}

// 2 argument comparison functions used with std::sort()

// calls datecmp with the less than parameter, for use with std::sort
bool datecmp_less_only(std::string query, std::string model){ return datecmp(query, model, -1); }
// calls datecmp with the less than parameter, for use with std::sort
bool datecmp_grtr_only(std::string a, std::string b){ return datecmp(a, b, 1); }
// used with std::sort, makes std::sort sort in descending order
bool int_sort_desc(int a, int b){ return a > b; }
// used with std::sort, makes std::sort sort in descending order
bool str_sort_desc(std::string a, std::string b){ return a > b; }

#endif
