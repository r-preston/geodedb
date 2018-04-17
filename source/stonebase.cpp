#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <ctime>

// this is the only file that actually includes sqlite3, for portability
#include <sqlite3.h>

#include "headers/dbaccesshelpers.h" // all helper functions that don't actively access the database live here
#include "headers/structures.h" // for collection_entry and mineral_entry
#include "headers/parsing.h" // functions related to parsing and manipulating user input and output

// used for casting a memory address for an std::vector through a void pointer to reach an sqlite3_exec()
// callback function in order to get table names...
// ...don't ask
typedef std::vector<std::string> table_type;

// this is the database access object, the reason all functions that access the database are in this one file
// all queries to the database use this object as the access point
sqlite3 *db;

const char * sort_items(std::vector<int> &rowids, std::string table, bool ascend, int sort_mask = 0)
{
    /*
    this function sorts rowids based on the column specified by sort_mask
    if ascend is true, the list is sorted in ascending order
    if ascend is false, the list is sorted in descending order

    sort_mask values:
        0 = Name
        1 = PurchaseNumber
        2 = Date
        3 = Cost
        4 = MineralType
     */
    std::string columns[5] = {"Name","PurchaseNumber","Date","Cost","MineralType"}; // columns corresponding to sort_mask
    std::string column = columns[sort_mask]; // set column to column name corresponding to sort_mask

    std::vector<std::string> str_values, str_unsort; // stores values of string type (all but cost and purchase number) if applicable
    std::vector<int> int_values, int_unsort; // stores cost/purchase number values if applicable
    std::vector<int> sorted_ids; // ids will be added in order to this according to the order of corresponding sorted values in int_values or str_values

    sqlite3_stmt *stmt; // sql query object
    std::string str_result; // stores result from sqlite3_column_text after being converted from const char *

    std::string sql = "SELECT "+column+" FROM "+table+" WHERE ROWID=?1;"; // sql query

    for(int id:rowids) // for each rowid to be sorted
    {
        int rc = 0; // error code
        sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL); // prepare statement
        sqlite3_bind_int(stmt, 1, id); // bind rowid to sql condition

        while(rc != SQLITE_DONE)
        {
            rc = sqlite3_step(stmt); // execute sql query
            if(rc == SQLITE_ROW) // data is ready for processing
            {
                if(sqlite3_column_type(stmt, 0) == SQLITE_INTEGER) // if column is of integer type
                {
                    int_values.push_back( sqlite3_column_int(stmt, 0) ); // add int to int_values
                }
                else if(sqlite3_column_type(stmt, 0) == SQLITE_TEXT) // if column is of text type
                {
                    // convert the text from const char * to std::string and save to str_result
                    str_result = std::string(reinterpret_cast<const char*>(
                        sqlite3_column_text(stmt, 0) // extract text from row
                    ));
                     // sort should be case-insensitive, so convert all string values to lowercase
                    std::transform(str_result.begin(), str_result.end(), str_result.begin(), ::tolower);
                    str_values.push_back( str_result ); // add value to str_values
                }
            }
            else if(rc != SQLITE_DONE) // error
            {
                // IT'S ON FIRE! RUN FOR IT!!
                sqlite3_finalize(stmt);
                return sqlite3_errmsg(db); // returns the error that caused sqlite_step to stop
            }
        }
        sqlite3_reset(stmt); // reset prepared statement
        sqlite3_clear_bindings(stmt); // clear bindings from sql statement
    }


    // before results are sorted, set unsorted vector to allow matching rowids later
    int_unsort = int_values;
    str_unsort = str_values;

    // the integer values are the ones with odd sort_mask values, so if sort_mask is odd sort_mask % 2 == 1
    if(sort_mask % 2 == 1) // if dealing with ints
    {

        if(ascend)
            std::sort(int_values.begin(), int_values.end()); // sort int_values in ascending order
        else
            std::sort(int_values.begin(), int_values.end(), int_sort_desc); // sort int_values in descending order
    }
    else if(sort_mask != 2) // dealing with strings (sortmask=2 is date)
    {
        if(ascend)
            std::sort(str_values.begin(), str_values.end()); // sort strings in ascending order
        else
            std::sort(str_values.begin(), str_values.end(), str_sort_desc); // sort strings in ascending order
    }
    else // dealing with dates
    {
        if(ascend)
            std::sort(str_values.begin(), str_values.end(), datecmp_less_only); // sort dates in ascending order
        else
            std::sort(str_values.begin(), str_values.end(), datecmp_grtr_only); // sort dates in ascending order
    }


    if(str_values.size()) // if string results have been processed
    {
        for(std::string value:str_values) // for each sorted string result
        {
            // find the position of this value in the unsorted list
            std::vector<std::string>::iterator it = std::find(str_unsort.begin(), str_unsort.end(), value);
            // convert this to an index position
            size_t index = std::distance(str_unsort.begin(), it);

            // get the rowid corresponding to this result and add it to the list of sorted rowids
            sorted_ids.push_back(rowids[index]);

            rowids.erase(rowids.begin() + index); // remove that rowid
            str_unsort.erase(it); // remove value corresponding to that rowid
        }
        if (sorted_ids.size() < rowids.size())
        {
            for (int rowid : rowids)
            {
                if (std::find(sorted_ids.begin(), sorted_ids.end(), rowid) == sorted_ids.end())
                {
                    sorted_ids.push_back(rowid);
                }
            }
        }
    }
    else // integer results have been processed
    {

        for(int value:int_values)
        {
            // find the position of this value in the unsorted list
            std::vector<int>::iterator it = std::find(int_unsort.begin(), int_unsort.end(), value);
            // convert this to an index position
            size_t index = std::distance(int_unsort.begin(), it);

            // get the rowid corresponding to this result and add it to the list of sorted rowids
            sorted_ids.push_back(rowids[index]);

            rowids.erase(rowids.begin() + index); // remove that rowid
            int_unsort.erase(it); // remove value corresponding to that rowid
        }
        if (sorted_ids.size() < rowids.size())
        {
            for (int rowid : rowids)
            {
                if (std::find(sorted_ids.begin(), sorted_ids.end(), rowid) == sorted_ids.end())
                {
                    sorted_ids.push_back(rowid);
                }
            }
        }
    }


    rowids = sorted_ids; // get rowid list to the sorted list. sorting complete

    return 0;
}

const char * get_collection_columns(int rowid, std::vector<std::string> columns, collection_entry &data, std::string tablename)
{
    /*
    this function retrieves columns from a row with ROWID matching rowid and saves them into &data
    the columns to extract are specified by columns
    if columns is empty, retrieve all columns
    */

    reset_object(data);
    std::string sql; // for constructing sql query
    sqlite3_stmt *stmt; // sql query object
    int rc = 0; // stores error code for query
    int int_result; // stores int-type column values
    std::string str_result; // stores string-type column values

    sql = "SELECT "; // we want to use the SQL SELECT statement

    if(columns.size() == 0)
    {
        // set columns to every column
        columns = {"ROWID","PurchaseNumber", "PurchaseNumberOrder", "Name", "MineralType", "Description", "Colours",
                   "Date", "PurchaseLocation", "Notes", "Cost", "Buyer", "CurrentLocation", "Texture", "Images"};
    }

    // add all of the columns to the query
    for(unsigned int i = 0; i < columns.size()-1; i++)
    {
        // add each value bar the last and add a comma to separate from the next value
        sql += columns[i];
        sql += ",";
    }
    sql += columns[columns.size()-1]; // no comma for last value

    sql += " FROM "+tablename+" WHERE ROWID=?1"; // set table to grab from and ROWID condition, leaving bind site to bind rowid to later

    sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL); // convert statement to byte code query

    sqlite3_bind_int(stmt, 1, rowid); // bind rowid to sql condition

    while(rc != SQLITE_DONE)
    {
        rc = sqlite3_step(stmt); // execute sql query
        if(rc == SQLITE_ROW) // data is ready for processing
        {

            for(int i = 0; i < sqlite3_column_count(stmt); i++) // for each column in the row returned
            {
                if(sqlite3_column_type(stmt, i) == SQLITE_INTEGER) // if column is of integer type
                {
                    // save column value to int_result
                    int_result = sqlite3_column_int(stmt, i);

                    // work out where in &data to put the value
                    // sqlite3_column_*() should bring out value in the same order as columns in the original sql query
                    // this means I can check what column I asked for using columns
                    if(columns[i] == "ROWID")
                        data.id = int_result;
                    else if(columns[i] == "PurchaseNumber")
                        data.purchase = int_result;
                    else if(columns[i] == "Cost")
                        data.cost = int_result;
                    else if(columns[i] == "Texture")
                        data.texture = int_result;
                    // no else clause, if it isn't one of these something has gone wrong and I don't want to wrongly assign values
                }
                else if(sqlite3_column_type(stmt, i) == SQLITE_TEXT) // if column is of text type
                {
                    // convert the text from const char * to std::string and save to str_result
                    str_result = std::string(reinterpret_cast<const char*>(
                        sqlite3_column_text(stmt, i) // extract text from row
                    ));

                    // work out where in &data to put the value
                    // sqlite3_column_*() should bring out value in the same order as columns in the original sql query
                    // this means I can check what column I asked for using columns
                    if(columns[i] == "PurchaseNumberOrder")
                        data.order = str_result;
                    else if(columns[i] == "Name")
                        data.name = str_result;
                    else if(columns[i] == "Description")
                        data.description = str_result;
                    else if(columns[i] == "Date")
                        data.date = str_result;
                    else if(columns[i] == "PurchaseLocation")
                        data.buy_location = str_result;
                    else if(columns[i] == "Notes")
                        data.notes = str_result;
                    else if(columns[i] == "Buyer")
                        data.buyer = str_result;
                    else if(columns[i] == "CurrentLocation")
                        data.now_location = str_result;
                    else if(columns[i] == "MineralType")
                        data.mineral = explode(str_result, ','); // mineral is a vector, so break text on commas
                    else if(columns[i] == "Colours")
                        data.colours = explode(str_result, ','); // colours is a vector, so break text on commas
                    else if(columns[i] == "Images")
                        data.images = explode(str_result, '\u001c'); // images is a vector, so break text on file seperator char (ascii 28, 0x1c)
                    // no else clause, if it isn't one of these something has gone wrong and I don't want to wrongly assign values
                }
                // if not integer or text, it's probably NULL so it can be ignored
            }

        }
        else if(rc != SQLITE_DONE) // error
        {
            // IT'S ON FIRE! RUN FOR IT!!
            sqlite3_finalize(stmt);
            return sqlite3_errmsg(db); // returns the error that caused sqlite_step to stop
        }
    }

    sqlite3_finalize(stmt);
    return 0;
}

const char * get_mineral_columns(int rowid, std::vector<std::string> columns, mineral_entry &data)
{
    /*
    this function retrieves columns from a row with ROWID matching rowid and saves them into &data
    the columns to extract are specified by columns
    if columns is empty, retrieve all columns
    */

    reset_object(data);
    std::string sql; // for constructing sql query
    sqlite3_stmt *stmt; // sql query object
    int rc = 0; // stores error code for query
    int int_result; // stores int-type column values
    std::string str_result; // stores string-type column values

    sql = "SELECT "; // we want to use the SQL SELECT statement

    if(columns.size() == 0)
    {
        // set columns to every column
        columns = {"ROWID", "Name", "Varieties", "Description", "Colours", "Structure", "Composition", "Category", "PageNumber", "Images"};
    }

    // add all of the columns to the query
    for(unsigned int i = 0; i < columns.size()-1; i++)
    {
        // add each value bar the last and add a comma to separate from the next value
        sql += columns[i];
        sql += ",";
    }
    sql += columns[columns.size()-1]; // no comma for last value

    sql += " FROM Minerals WHERE ROWID=?1"; // set table to grab from and ROWID condition, leaving bind site to bind rowid to later

    sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL); // convert statement to byte code query

    sqlite3_bind_int(stmt, 1, rowid); // bind rowid to sql condition

    while(rc != SQLITE_DONE)
    {
        rc = sqlite3_step(stmt); // execute sql query
        if(rc == SQLITE_ROW) // data is ready for processing
        {

            for(int i = 0; i < sqlite3_column_count(stmt); i++) // for each column in the row returned
            {
                if(sqlite3_column_type(stmt, i) == SQLITE_INTEGER) // if column is of integer type
                {
                    // save column value to int_result
                    int_result = sqlite3_column_int(stmt, i);

                    // work out where in &data to put the value
                    // sqlite3_column_*() should bring out value in the same order as columns in the original sql query
                    // this means I can check what column I asked for using columns
                    if(columns[i] == "ROWID")
                        data.id = int_result;
                    else if(columns[i] == "PageNumber")
                        data.pagenumber = int_result;
                    // no else clause, if it isn't one of these something has gone wrong and I don't want to wrongly assign values
                }
                else if(sqlite3_column_type(stmt, i) == SQLITE_TEXT) // if column is of text type
                {
                    // convert the text from const char * to std::string and save to str_result
                    str_result = std::string(reinterpret_cast<const char*>(
                        sqlite3_column_text(stmt, i) // extract text from row
                    ));

                    // work out where in &data to put the value
                    // sqlite3_column_*() should bring out value in the same order as columns in the original sql query
                    // this means I can check what column I asked for using columns
                    if(columns[i] == "Name")
                        data.name = str_result;
                    else if(columns[i] == "Description")
                        data.description = str_result;
                    else if(columns[i] == "Structure")
                        data.structure = str_result;
                    else if(columns[i] == "Composition")
                        data.composition = str_result;
                    else if(columns[i] == "Category")
                        data.category = str_result;
                    else if(columns[i] == "Varieties")
                        data.varieties = explode(str_result, ','); // varieties is a vector, so break text on commas
                    else if(columns[i] == "Colours")
                        data.colours = explode(str_result, ','); // colours is a vector, so break text on commas
                    else if(columns[i] == "Images")
                        data.images = explode(str_result, '\u001c'); // images is a vector, so break text on file seperator char (ascii 28, 0x1c)
                    // no else clause, if it isn't one of these something has gone wrong and I don't want to wrongly assign values
                }
                // if not integer or text, it's probably NULL so it can be ignored
            }

        }
        else if(rc != SQLITE_DONE) // error
        {
            // IT'S ON FIRE! RUN FOR IT!!
            sqlite3_finalize(stmt);
            return sqlite3_errmsg(db); // returns the error that caused sqlite_step to stop
        }
    }

    sqlite3_finalize(stmt);
    return 0;
}

const char * search_date(std::vector<int> &results, const std::string tablename, std::string datestring, int comp, bool andor)
{
    /*
    Because sqlite does not have any built-in sort functions for date, I made my own - this function.
    It reads tablename and one by one compares the date to datestring.
    If the date matches the query, it is added to results.
    */
    std::string sql, tbldate; // sql is sql query, tbldate is the date to compare read from the table
    sqlite3_stmt *stmt; // sql query object
    std::vector<int> and_results; // if the results are specified with AND, we remove items from results. and_results will become results later
    int rc = 0; // stores error code for query
    int rowid; // stores ROWID
    bool isdate = false; // stores result of comparing tbldate to datestring

    // create sql query to extract dates and ROWIDs from tablename
    sql = "SELECT ROWID, Date FROM "+tablename+";";

    // create sql query object
    sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL);

    // while there are still results to process
    while(rc != SQLITE_DONE)
    {
        rc = sqlite3_step(stmt); // execute query
        if(rc == SQLITE_ROW) // if there are some results
        {
            if(sqlite3_column_count(stmt) > 0)
            {
                // convert the const char* given by sqlite to an std::string
                tbldate = std::string(reinterpret_cast<const char*>(
                    sqlite3_column_text(stmt, 1) // extract date from row
                ));
                isdate = datecmp(tbldate, datestring, comp); // compare the two dates and set isdate as the result

                rowid = sqlite3_column_int(stmt, 0); // extract ROWID
                if(andor) // if comparing with the boolean OR operator
                {
                    if(isdate && std::find(results.begin(), results.end(), rowid) == results.end())
                    {
                        // if the date fits the query and the ROWID is not already in the list
                        // because we don't want duplicates
                        results.push_back(rowid); // add ROWID to results
                    }
                }
                else if(results.size()) // if comparing with the boolean AND operator
                {
                    if(isdate && std::find(results.begin(), results.end(), rowid) != results.end())
                    {
                        // if the date fits the query and it already in the list, we add it to the new list
                        // if the corresponding ROWID is not in the list, it does not match <other query> AND <date query>, so is dropped
                        and_results.push_back(rowid);
                    }
                }
                else if(isdate)
                {
                    and_results.push_back(rowid);
                }
            }
        }
        else if(rc != SQLITE_DONE) // error
        {
            // IT'S ON FIRE! RUN FOR IT!!
            sqlite3_finalize(stmt);
            return sqlite3_errmsg(db); // returns the error that caused sqlite_step to stop
        }
    }

    // if comparing with AND, set results to and_results
    if(!andor)
        results = and_results;

    // delete the sql query object now we've finished with it
    sqlite3_finalize(stmt);

    return 0;
}

const char * search_collection(collection_entry args, std::string any, int argnum, const std::string tabnam, search_result &matched, std::vector<int> compares = {0,0,0}, bool s_or = true)
{
    /*
    this function searches the collection-type tables with queries coming from any (for hits in any column) and column-specific queries from args
    if a member of args is not empty, it is treated as a query

    the ROWID of matching rows is added to the vector search_results::results in the search_result structure matched

    argnum is the amount of different search parameters (excluding date and any)

    s_or is true if multiple queries are joined by a boolean OR, and false if AND

    compares stores comparison types for columns that can be used with the < or > operators
    compares has specific format - {<number>,<date>,<cost>} 0=equals,-1=less than, 1=more than
    */

    int rc = 0; // error code for sql query
    int nth = 0; // number of search parameters processed
    std::string sql, comp; // sql is sql query string
    sqlite3_stmt *stmt; // sql query object
    std::vector<std::string> bindings; // the order in which to bind values to *stmt with sqlite3_bind_*

    matched.results.clear(); // clears previous search matched.results

    matched.tablename = tabnam; // set the name of the table the results have come from

    if(!argnum && any == "") // if there are no search parameters, we fetch all rows
    {
        if(args.date == "")
        {
            sql = "SELECT ROWID FROM "+tabnam+";"; // sql query will return all rows
            sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL); // prepare statement
            while(rc != SQLITE_DONE) // while results left
            {
                rc = sqlite3_step(stmt); // execute the query
                if(rc == SQLITE_ROW) // result to be processed
                {
                    if(sqlite3_column_count(stmt) > 0) // if the row is not empty
                    {
                        matched.results.push_back(sqlite3_column_int(stmt, 0)); // add the ROWID to matched.results
                    }
                }
                else if(rc == SQLITE_ERROR) // oh no - an error
                {
                    // IT'S ON FIRE! RUN FOR IT!!
                    sqlite3_finalize(stmt);
                    return sqlite3_errmsg(db); // returns the error that caused sqlite_step to stop
                }
            }
            sqlite3_finalize(stmt); // delete the query object when we are finished
        }
        else
        {
            // date has been given as a condition - launch the custom date search function
            search_date(matched.results, tabnam, args.date, compares[1], s_or);
        }

        // sort the results according to the pre-existing search mask
        sort_items(matched.results, tabnam, matched.sort_ascending, matched.sort_mask);


        return 0; // abort, no further searching needed
    }

    // set the boolean keyword
    if(s_or) comp = " OR ";
    else comp = " AND ";

    sql = "SELECT ROWID FROM "+tabnam+" WHERE ";

    //{ creating the sql query - this is of the form SELECT ROWID FROM <TABLE> WHERE [condition LIKE/=/</> value]
    //{

    //
    // this if block is treated as a model for all blocks in this if/else if/else block
    // apart from the first one, code will only be commented if it is different to this one
    // this is so I don't repeat the same comments a dozen times
    //
    if(args.purchase != -1) // if a value for purchase number if given
    {
        sql += "PurchaseNumber"; // add column name as condition
        if(compares[0] == -1) sql += "<"; // extract from comp the correct comparison character
        else if(compares[0] == 1) sql += ">";
        else sql += "=";
        // in sqlite, ?n stands in for a value, which can later be bound to that site using sqlite_bind*()
        sql += "?" + std::to_string(nth+1);
        // we push an identifier to bindings so we know what order to bind values in later
        bindings.push_back("num");
        // increase number of queries processed by 1
        nth++;
        // if argnum is greater that nth, there are still conditions left
        // therefore, we add AND or OR in
        if(argnum > nth) sql += comp;
    }
    if(args.cost != -1)
    {
        sql += "Cost";
        if(compares[2] == -1) sql += "<";
        else if(compares[2] == 1) sql += ">";
        else sql += "=";
        sql += "?" + std::to_string(nth+1);
        bindings.push_back("cost");
        nth++;
        if(argnum > nth) sql += comp;
    }
    if(args.texture != -1)
    {
        sql += "Texture=?"+std::to_string(nth+1); // texture only uses =
        bindings.push_back("texture");
        nth++;
        if(argnum > nth) sql += comp;
    }
    if(args.name != "")
    {
        sql += "Name LIKE ?"+std::to_string(nth+1); // all string values use the sql keyword 'LIKE' to search for a pattern in the column
        bindings.push_back("name");
        nth++;
        if(argnum > nth) sql += comp;
    }
    if(args.description != "")
    {
        sql += "Description LIKE ?"+std::to_string(nth+1);
        bindings.push_back("description");
        nth++;
        if(argnum > nth) sql += comp;
    }
    if(args.buyer != "")
    {
        sql += "Buyer LIKE ?"+std::to_string(nth+1);
        bindings.push_back("buyer");
        nth++;
        if(argnum > nth) sql += comp;
    }
    if(args.notes != "")
    {
        sql += "Notes LIKE ?"+std::to_string(nth+1);
        bindings.push_back("notes");
        nth++;
        if(argnum > nth) sql += comp;
    }
    if(args.buy_location != "")
    {
        sql += "PurchaseLocation LIKE ?"+std::to_string(nth+1);
        bindings.push_back("from");
        nth++;
        if(argnum > nth) sql += comp;
    }
    if(args.now_location != "")
    {
        sql += "CurrentLocation LIKE ?"+std::to_string(nth+1);
        bindings.push_back("location");
        nth++;
        if(argnum > nth) sql += comp;
    }
    for(unsigned int i = 0; i < args.mineral.size(); i++) // minerals can have multiple values, so iterate through list to add the MineralType condition multiple times
    {
        sql += "MineralType LIKE ?"+std::to_string(nth+1);
        bindings.push_back("mineral");
        nth++;
        if(argnum > nth) sql += comp;
    }
    for(unsigned int i = 0; i < args.colours.size(); i++) // ditto as above
    {
        sql += "Colours LIKE ?"+std::to_string(nth+1);
        bindings.push_back("colour");
        nth++;
        if(argnum > nth) sql += comp;
    }
    //}
    if(any != "")
    {
        // any matches for a pattern in all of the columns in fields
        std::vector<std::string> fields = {"Name", "MineralType", "Description", "Colours", "PurchaseLocation", "Notes", "Buyer", "CurrentLocation"};

        // if argnum > 0, there have been previous arguments so add in AND or OR
        if(argnum){ sql += comp;}
        // any always uses OR, so put this condition in brackets so it evaluates properly when used with other conditions and AND
        sql += "(";
        // for each column name in fields, create condition and add binding point
        for(auto n:fields)
        {
            // <column name> LIKE <value> OR ... and so on for each column name
            sql += n+" LIKE ?"+std::to_string(nth+1);
            if(n != "CurrentLocation") sql += " OR ";
            bindings.push_back("any");
            nth++; // any contributes to nth because it uses binding points
        }
        sql += ")"; // end block
    }
    sql += ";"; // end sql query
    //}
    // prepare the statement
    sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL);

    // begin the binding of values
    // iterate through all bindings, the magnitude of which is stored by nth
    for(int i = 0; i < nth; i++)
    {
        // for each identifier, bind the appropriate value
        if(bindings[i] == "num")
            sqlite3_bind_int(stmt, i+1, args.purchase);
        else if(bindings[i] == "cost")
            sqlite3_bind_int(stmt, i+1, args.cost);
        else if(bindings[i] == "texture")
            sqlite3_bind_int(stmt, i+1, args.texture);
        else if(bindings[i] == "name")
            sqlite3_bind_text(stmt, i+1, args.name.c_str(), args.name.size(), SQLITE_STATIC); // string values a converted to char* by c_str(), size must be given for char lists
        else if(bindings[i] == "description")
            sqlite3_bind_text(stmt, i+1, args.description.c_str(), args.description.size(), SQLITE_STATIC);
        else if(bindings[i] == "buyer")
            sqlite3_bind_text(stmt, i+1, args.buyer.c_str(), args.buyer.size(), SQLITE_STATIC);
        else if(bindings[i] == "notes")
            sqlite3_bind_text(stmt, i+1, args.notes.c_str(), args.notes.size(), SQLITE_STATIC);
        else if(bindings[i] == "from")
            sqlite3_bind_text(stmt, i+1, args.buy_location.c_str(), args.buy_location.size(), SQLITE_STATIC);
        else if(bindings[i] == "location")
            sqlite3_bind_text(stmt, i+1, args.now_location.c_str(), args.now_location.size(), SQLITE_STATIC);
        else if(bindings[i] == "any")
            sqlite3_bind_text(stmt, i+1, any.c_str(), any.size(), SQLITE_STATIC);
        else if(bindings[i] == "mineral")
        {
            // for items in lists, items are deleted as they are added so SQLITE_TRANSIENT is specified to make sqlite make a copy of the string before continuing
            sqlite3_bind_text(stmt, i+1, args.mineral[0].c_str(), args.mineral[0].size(), SQLITE_TRANSIENT);
            args.mineral.erase(args.mineral.begin()); // erase item so all items in the list are bound. order it irrelevant
        }
        else if(bindings[i] == "colour")
        {
            sqlite3_bind_text(stmt, i+1, args.colours[0].c_str(), args.colours[0].size(), SQLITE_TRANSIENT);
            args.colours.erase(args.colours.begin());
        }
    }

    // while there are still values to process
    while(rc != SQLITE_DONE)
    {
        rc = sqlite3_step(stmt); // execute sql query
        if(rc == SQLITE_ROW) // data is ready for processing
        {
            if(sqlite3_column_count(stmt) > 0) // if column is not empty
                matched.results.push_back( sqlite3_column_int(stmt, 0) ); // add ROWID to matched.results
        }
        else if(rc != SQLITE_DONE) // error
        {
            // IT'S ON FIRE! RUN FOR IT!!
            sqlite3_finalize(stmt);
            return sqlite3_errmsg(db); // returns the error that caused sqlite_step to stop
        }
    }

    // delete object and deallocate memory
    sqlite3_finalize(stmt);

    if(args.date != "") // date has been given as a condition - launch the custom date search function
        search_date(matched.results, tabnam, args.date, compares[1], s_or);

    // sort the results according to the pre-existing search mask
    sort_items(matched.results, tabnam, matched.sort_ascending, matched.sort_mask);

    return 0;
}

const char * search_minerals(mineral_entry args, std::string any, int argnum, search_result &matched, bool s_or = true)
{
    /*
    this function searches the Minerals table with queries coming from any (for hits in any column) and column-specific queries from args
    if a member of args is not empty, it is treated as a query

    the ROWID of matching rows is added to the vector matched.results

    argnum is the amount of different search parameters (excluding date and any)

    s_or is true if multiple queries are joined by a boolean OR, and false if AND
    */

    int rc = 0; // error code from sqlite_step
    int nth = 0; // number of conditions processed
    std::string sql, comp;
    sqlite3_stmt *stmt; // sql query object
    std::vector<std::string> bindings; // list of identifiers for value binding (see: sqlite_bind_*())

    matched.results.clear(); // clears previous search results

    matched.tablename = "Minerals"; // set the name of the table the results come from

    if(!argnum && any == "")
    {
        // if no conditions are provided, grab the lot
        sql = "SELECT ROWID FROM Minerals;"; // select all rows
        sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL); // prepare the statement
        while(rc != SQLITE_DONE) // while rows remains unprocessed, carry on processing
        {
            rc = sqlite3_step(stmt); // execute sql query
            if(rc == SQLITE_ROW) // data ready for processing
            {
                if(sqlite3_column_count(stmt) > 0) // if column is not empty
                {
                    matched.results.push_back(sqlite3_column_int(stmt, 0)); // add ROWID to matched.results
                }
            }
            else if(rc == SQLITE_ERROR) // error
            {
                // IT'S ON FIRE! RUN FOR IT!!
                sqlite3_finalize(stmt);
                return sqlite3_errmsg(db); // returns the error that caused sqlite_step to stop
            }
        }
        sqlite3_finalize(stmt); // end the query and delete the query object
        return 0;
    }

    if(s_or) comp = " OR "; // if boolean is OR, set comp value to OR
    else comp = " AND "; // otherwise AND

    // select ROWIDs from Minerals, ready for conditions to be added
    sql = "SELECT ROWID FROM Minerals WHERE ";

    //{ here be sql string creation
    //{
    if(args.name != "")
    {
        sql += "Name LIKE ?"+std::to_string(nth+1); // the keyword LIKE matches the given pattern in the column
        // ?n is a binding site that will have a value bound to later
        bindings.push_back("name"); // push an identifier to bindings to the program knows what order to bind value in
        nth++; // argument processed
        if(argnum > nth) sql += comp; // if argnum > nth, there are still more conditions to be processed. therefore, add OR or AND
    }
    // all the rest are the same as the one above
    if(args.description != "")
    {
        sql += "Description LIKE ?"+std::to_string(nth+1);
        bindings.push_back("description");
        nth++;
        if(argnum > nth) sql += comp;
    }
    if(args.category != "")
    {
        sql += "Category LIKE ?"+std::to_string(nth+1);
        bindings.push_back("category");
        nth++;
        if(argnum > nth) sql += comp;
    }
    if(args.structure != "")
    {
        sql += "Structure LIKE ?"+std::to_string(nth+1);
        bindings.push_back("structure");
        nth++;
        if(argnum > nth) sql += comp;
    }
    if(args.composition != "")
    {
        sql += "Composition LIKE ?"+std::to_string(nth+1);
        bindings.push_back("composition");
        nth++;
        if(argnum > nth) sql += comp;
    }
    for(unsigned int i = 0; i < args.varieties.size(); i++)
    {
        sql += "Varieties LIKE ?"+std::to_string(nth+1);
        bindings.push_back("varieties");
        nth++;
        if(argnum > nth) sql += comp;
    }
    for(unsigned int i = 0; i < args.colours.size(); i++)
    {
        sql += "Colours LIKE ?"+std::to_string(nth+1);
        bindings.push_back("colour");
        nth++;
        if(argnum > nth) sql += comp;
    }

    //}
    if(any != "")
    {
        // any tries to match a pattern in any of the following columns:
        std::vector<std::string> fields = {"Name", "Category", "Structure", "Description", "Colours", "Composition", "Varieties"};

        // if argnum != 0, add AND or OR before this condition
        if(argnum){ sql += comp; }
        sql += "("; // any always uses OR, so for proper parsing wrap these conditions in brackets
        for(auto n:fields) // for each column, add a condition to match the given pattern
        {
            sql += n+" LIKE ?"+std::to_string(nth+1);
            if(n != "Varieties") sql += " OR "; // if it's not the last column yet, add OR
            bindings.push_back("any"); // any will have it's pattern bound multiple times, so multiple identifiers are needed
            nth++; // argument processed
        }
        sql += ")"; // end any condition block
    }
    sql += ";"; // end sql query
    //}

    // prepare the statement
    sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL);

    // for each ?n binding site, bind the appropriate value
    for(int i = 0; i < nth; i++)
    {
        if(bindings[i] == "name")
            sqlite3_bind_text(stmt, i+1, args.name.c_str(), args.name.size(), SQLITE_STATIC);
        else if(bindings[i] == "description")
            sqlite3_bind_text(stmt, i+1, args.description.c_str(), args.description.size(), SQLITE_STATIC);
        else if(bindings[i] == "structure")
            sqlite3_bind_text(stmt, i+1, args.structure.c_str(), args.structure.size(), SQLITE_STATIC);
        else if(bindings[i] == "composition")
            sqlite3_bind_text(stmt, i+1, args.composition.c_str(), args.composition.size(), SQLITE_STATIC);
        else if(bindings[i] == "category")
            sqlite3_bind_text(stmt, i+1, args.category.c_str(), args.category.size(), SQLITE_STATIC);
        else if(bindings[i] == "any")
            sqlite3_bind_text(stmt, i+1, any.c_str(), any.size(), SQLITE_STATIC);
        else if(bindings[i] == "varieties")
        {
            // for list items, each value in the list needs to be independently bound
            sqlite3_bind_text(stmt, i+1, args.varieties[0].c_str(), args.varieties[0].size(), SQLITE_TRANSIENT); // as items are deleted, SQLITE_TRANSIENT tells sqlite to make it's own copy
            // delete the item just processed
            args.varieties.erase(args.varieties.begin());
        }
        else if(bindings[i] == "colour")
        {
            sqlite3_bind_text(stmt, i+1, args.colours[0].c_str(), args.colours[0].size(), SQLITE_TRANSIENT);
            args.colours.erase(args.colours.begin());
        }
    }

    while(rc != SQLITE_DONE)
    {
        rc = sqlite3_step(stmt); // execute sql query
        if(rc == SQLITE_ROW) // data is ready for processing
        {
            if(sqlite3_column_count(stmt) > 0) // if column is not empty
                matched.results.push_back( sqlite3_column_int(stmt, 0) ); // add ROWID to matched.results
        }
        else if(rc != SQLITE_DONE) // error
        {
            // IT'S ON FIRE! RUN FOR IT!!
            sqlite3_finalize(stmt);
            return sqlite3_errmsg(db); // returns the error that caused sqlite_step to stop
        }
    }

    // end the query
    sqlite3_finalize(stmt);

    // sort the results according to the pre-existing search mask
    sort_items(matched.results, "Minerals", matched.sort_ascending, matched.sort_mask);

    return 0;
}

const char * get_table_records(const std::string tablename, int &counts)
{
    /*
    This function retrieves the number of rows in the table 'tablename'
    */
    int rc = 0; // stores error code for sqlite_step
    std::string sql; // sql query string
    sqlite3_stmt *stmt; // query object

    // COUNT(*) gets the number of entries in the table
    sql = "SELECT COUNT(*) FROM "+tablename+";";

    // prepare the statement
    sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL);

    // this loop should return only one single value - the number of rows in the table
    while(rc != SQLITE_DONE)
    {
        rc = sqlite3_step(stmt); // execute statement
        if(rc == SQLITE_ROW) // data ready
        {
            // set counts to the number of rows
            if(sqlite3_column_count(stmt) > 0) counts = sqlite3_column_int(stmt, 0);
        }
        else if(rc == SQLITE_ERROR) // error
        {
            // IT'S ON FIRE! RUN FOR IT!!
            sqlite3_finalize(stmt);
            return sqlite3_errmsg(db); // returns the error that caused sqlite_step to stop
        }
    }

    // finalise the query and end
    sqlite3_finalize(stmt);
    return 0;
}

const char * new_collection_row(collection_entry &colEntry, const std::string tablename, const std::string image_path)
{
    /*
    This function adds a new entry into a collection-type table specified by tablename
    The values to add are extracted from colEntry

    Picks up NULL indicators such as -1 or ~
    */
    std::string sql, minerals, colours, images; // sql is query, other strings are used to convert a list of items into a single string
    int rc; // error code
    sqlite3_stmt *stmt; // sql query object

    // bind values into every column
    // ROWID is automatically generated, so is not included here
    // ?n indicates a binding site corresponding to a column
    // the correct value will be bound later
    // this prevents sql injection
    sql = "INSERT INTO "+tablename+" (PurchaseNumber,PurchaseNumberOrder,Name,MineralType,Description,Colours,Date,"\
          "PurchaseLocation,Notes,Cost,Buyer,CurrentLocation,Images,Texture)"\
          " VALUES (?1,?2,?3,?4,?5,?6,?7,?8,?9,?10,?11,?12,?13,?14);";

    // prepare the statement
    sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL);

    //{ bindings in here
    // in this block of if/else statements, every member of colEntry is checked
    // if it is not NULL, bind the value to the sqlite query
    // otherwise, bind NULL to that column
    // each column has one binding site
    /*====================================================*/
    if(colEntry.purchase == -1)
        sqlite3_bind_null(stmt, 1);
    else
        sqlite3_bind_int(stmt, 1, colEntry.purchase);

    /*====================================================*/
    if(colEntry.order == "~")
        sqlite3_bind_null(stmt, 2);
    else
        sqlite3_bind_text(stmt, 2, colEntry.order.c_str(), colEntry.order.size(), SQLITE_STATIC);

    /*====================================================*/
    if(colEntry.name == "~")
        sqlite3_bind_null(stmt, 3);
    else
        sqlite3_bind_text(stmt, 3, colEntry.name.c_str(), colEntry.name.size(), SQLITE_STATIC);

    /*====================================================*/
    // convert the collection_entry::mineral list to a string of comma separated values
    minerals = vec_to_str(colEntry.mineral, true);
    if(minerals == "~")
        sqlite3_bind_null(stmt, 4);
    else
        sqlite3_bind_text(stmt, 4, minerals.c_str(), minerals.size(), SQLITE_STATIC);

    /*====================================================*/
    if(colEntry.description == "~")
        sqlite3_bind_null(stmt, 5);
    else
        sqlite3_bind_text(stmt, 5, colEntry.description.c_str(), colEntry.description.size(), SQLITE_STATIC);

    /*====================================================*/
    // convert the collection_entry::colours list to a string of comma separated values
    colours = vec_to_str(colEntry.colours, true);
    if(colours == "~")
        sqlite3_bind_null(stmt, 6);
    else
        sqlite3_bind_text(stmt, 6, colours.c_str(), colours.size(), SQLITE_STATIC);

    /*====================================================*/
    if(colEntry.date == "~")
        sqlite3_bind_null(stmt, 7);
    else
        sqlite3_bind_text(stmt, 7, colEntry.date.c_str(), colEntry.date.size(), SQLITE_STATIC);

    /*====================================================*/
    if(colEntry.buy_location == "~")
        sqlite3_bind_null(stmt, 8);
    else
        sqlite3_bind_text(stmt, 8, colEntry.buy_location.c_str(), colEntry.buy_location.size(), SQLITE_STATIC);

    /*====================================================*/
    if(colEntry.notes == "~")
        sqlite3_bind_null(stmt, 9);
    else
        sqlite3_bind_text(stmt, 9, colEntry.notes.c_str(), colEntry.notes.size(), SQLITE_STATIC);

    /*====================================================*/
    if(colEntry.cost == -1)
        sqlite3_bind_null(stmt, 10);
    else
        sqlite3_bind_int(stmt, 10, colEntry.cost);

    /*====================================================*/
    if(colEntry.buyer == "~")
        sqlite3_bind_null(stmt, 11);
    else
        sqlite3_bind_text(stmt, 11, colEntry.buyer.c_str(), colEntry.buyer.size(), SQLITE_STATIC);

    /*====================================================*/
    if(colEntry.now_location == "~")
        sqlite3_bind_null(stmt, 12);
    else
        sqlite3_bind_text(stmt, 12, colEntry.now_location.c_str(), colEntry.now_location.size(), SQLITE_STATIC);

    /*====================================================*/
    // replace all forward slashes with backslashes in the image path
    for(unsigned int i = 0; i < colEntry.images.size(); i++)
    {
        sanitise_path(colEntry.images[i]);
        colEntry.images[i] = assimilate_image(image_path, colEntry.images[i]);
    }
    // convert the collection_entry::images list to a string of comma separated values, as commas could appear in file names but pipes cannot
    images = vec_to_sepstring(colEntry.images, true);
    if(images == "~")
        sqlite3_bind_null(stmt, 13);
    else
        sqlite3_bind_text(stmt, 13, images.c_str(), images.size(), SQLITE_STATIC);

    /*====================================================*/
    if(colEntry.texture == -1)
        sqlite3_bind_null(stmt, 14);
    else
        sqlite3_bind_int(stmt, 14, colEntry.texture);
    //}

    // execute the sql query. adding a value returns no data, so no handling needed
    rc = sqlite3_step(stmt);

    // end query
    sqlite3_finalize(stmt);

    if(rc != SQLITE_DONE) // an error occurred
    {
        return sqlite3_errmsg(db); // returns the error that caused sqlite_step to stop
    }
    return 0;
}

const char * new_mineral_row(mineral_entry &minEntry, const std::string image_path)
{
    /*
    This function adds a new entry into a collection-type table specified by tablename
    The values to add are extracted from colEntry

    Picks up NULL indicators such as -1 or ~
    */
    std::string sql, varieties, colours, images; // sql is query, other strings are used to convert a list of items into a single string
    int rc; // error code
    sqlite3_stmt *stmt; // sql query object

    // bind values into every column
    // ROWID is automatically generated, so is not included here
    // ?n indicates a binding site corresponding to a column
    // the correct value will be bound later
    // this prevents sql injection
    sql = "INSERT INTO Minerals (Name,Varieties,Description,Colours,Structure,"\
          "Composition,Images,Category,PageNumber) VALUES (?1,?2,?3,?4,?5,?6,?7,?8,?9);";

    // prep statement
    sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL);

    //{ bindings in here

    // in this block of if/else statements, every member of minEntry is checked
    // if it is not NULL, bind the value to the sqlite query
    // otherwise, bind NULL to that column
    // each column has one binding site
    /*====================================================*/
    if(minEntry.name == "~") // if this string stands for a NULL value
        sqlite3_bind_null(stmt, 1); // bind NULL
    else
        sqlite3_bind_text(stmt, 1, minEntry.name.c_str(), minEntry.name.size(), SQLITE_STATIC); // else bind text value

    /*====================================================*/
    // convert the mineral_entry::varieties list to a string of comma separated values
    varieties = vec_to_str(minEntry.varieties, true);
    if(varieties == "~")
        sqlite3_bind_null(stmt, 2);
    else
        sqlite3_bind_text(stmt, 2, varieties.c_str(), varieties.size(), SQLITE_STATIC);

    /*====================================================*/
    if(minEntry.description == "~")
        sqlite3_bind_null(stmt, 3);
    else
        sqlite3_bind_text(stmt, 3, minEntry.description.c_str(), minEntry.description.size(), SQLITE_STATIC);

    /*====================================================*/
    // convert the mineral_entry::colours list to a string of comma separated values
    colours = vec_to_str(minEntry.colours, true);
    if(colours == "~")
        sqlite3_bind_null(stmt, 4);
    else
        sqlite3_bind_text(stmt, 4, colours.c_str(), colours.size(), SQLITE_STATIC);

    /*====================================================*/
    if(minEntry.structure == "~")
        sqlite3_bind_null(stmt, 5);
    else
        sqlite3_bind_text(stmt, 5, minEntry.structure.c_str(), minEntry.structure.size(), SQLITE_STATIC);

    /*====================================================*/
    if(minEntry.composition == "~")
        sqlite3_bind_null(stmt, 6);
    else
        sqlite3_bind_text(stmt, 6, minEntry.composition.c_str(), minEntry.composition.size(), SQLITE_STATIC);

    /*====================================================*/
    // replace all forward slashes with backslashes in the image path
    for(unsigned int i = 0; i < minEntry.images.size(); i++)
    {
        sanitise_path(minEntry.images[i]);
        minEntry.images[i] = assimilate_image(image_path, minEntry.images[i]);
    }
    // convert the mineral_entry::images list to a string of pipe separated values, as commas could appear in file names but pipes cannot
    images = vec_to_sepstring(minEntry.images, true);
    if(images == "~")
        sqlite3_bind_null(stmt, 7);
    else
        sqlite3_bind_text(stmt, 7, images.c_str(), images.size(), SQLITE_STATIC);

    /*====================================================*/
    if(minEntry.category == "~")
        sqlite3_bind_null(stmt, 8);
    else
        sqlite3_bind_text(stmt, 8, minEntry.category.c_str(), minEntry.category.size(), SQLITE_STATIC);

    /*====================================================*/
    if(minEntry.pagenumber == -1)
        sqlite3_bind_null(stmt, 9);
    else
        sqlite3_bind_int(stmt, 9, minEntry.pagenumber);

    /*====================================================*/

    //}}
    // execute the sql query. adding a value returns no data, so no handling needed
    rc = sqlite3_step(stmt);

    // end query
    sqlite3_finalize(stmt);

    if(rc != SQLITE_DONE) // an error occurred
    {
        return sqlite3_errmsg(db); // returns the error that caused sqlite_step to stop
    }
    return 0;
}

const char * open_database(std::string db_path)
{
    /*
    this function is called when the program is first opened to create a link to the database for all future queries
    it opens the database, creates the sqlite3 db object and creates the table 'Minerals' if it does not already exist
    */
    char *zErrMsg = 0; // error message from sqlite query
    std::string sql;
    int rc; // error code from sqlite_open
    int rcc = 1; // error code from sqlite_exec

    rc = sqlite3_open((db_path+"Stones.db").c_str(), &db); // open and create a connection to the database Stones.db
    if(rc) // an error occurred, could not open the database :(
        return sqlite3_errmsg(db); // returns the error that caused sqlite_open to abort

    // the table Minerals is a constant. if it does not exist when the database is opened, create it
    sql = "CREATE TABLE IF NOT EXISTS Minerals ("\
          "Name TEXT,"\
          "Varieties TEXT,"\
          "Description TEXT,"\
          "Colours TEXT,"\
          "Structure TEXT,"\
          "Composition TEXT,"\
          "Images TEXT,"\
          "Category TEXT,"\
          "PageNumber INTEGER);";

    // execute table creating command
    rcc = sqlite3_exec(db, sql.c_str(), empty_callback, 0, &zErrMsg);
    if(rcc != SQLITE_OK) // error
    {
        return zErrMsg; // return pointer to a more detailed error message
    }

    return 0;
}

const char * does_exist(const std::string table, int num, std::string matchstring, int &id)
{
    /*
    this function checks if an item already exists in a table, for the purposes of establishing if an object the user is about to add is a duplicate
    for the table Minerals, if a row exists with the same Name as matchstring, &id is set to the rowid of the matched item
    for collection-type tables, if a row exists with the same PurchaseNumber as num and the same PurchaseNumberOrder as matchstring, &id is set to the rowid of the matched item
    The function returns 0, or an error message if an error occurs
    */

    sqlite3_stmt *stmt; // sqlite query object
    std::string sql; // sql query
    int rc = 0; // error code

    // set rowid to -1, this is what will be returned if the item does not exists
    // rowids are always positive, so -1 indicates nothing was found
    id = -1;

    if(table == "Minerals")
    {
        // in the minerals table, look for an item that has the same name
        sql = "SELECT ROWID FROM Minerals WHERE Name=?1;"; // sql query to achieve the above
        sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL); // prepare query
        sqlite3_bind_text(stmt, 1, matchstring.c_str(), matchstring.size(), SQLITE_STATIC); // bind name into the query
    }
    else
    {
        // in collection-type tables, look for an item with the same purchase number and order in purchase
        sql = "SELECT ROWID FROM "+table+" WHERE PurchaseNumber=?1 AND PurchaseNumberOrder=?2;";
        sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL); // prepare query
        sqlite3_bind_int(stmt, 1, num); // bind purchase number to sql condition
        sqlite3_bind_text(stmt, 2, matchstring.c_str(), matchstring.size(), SQLITE_STATIC); // bind order in purchase into the query
    }

    while(rc != SQLITE_DONE)
    {
        rc = sqlite3_step(stmt); // execute sql query

        if(rc == SQLITE_ROW) // data is ready for processing
        {
            id = sqlite3_column_int(stmt, 0); // get id of matched item from query
        }
        else if(rc != SQLITE_DONE) // error
        {
            // IT'S ON FIRE! RUN FOR IT!!
            sqlite3_finalize(stmt);
            return sqlite3_errmsg(db); // returns the error that caused sqlite_step to stop
        }
    }

    return 0;
}

const char * update_mineral_item(mineral_entry to_update, mineral_entry &active_item, std::vector<std::string> update_columns , std::string image_path)
{
    /*
    this function updates columns of the active item in the Minerals table
    active_item is a pointer to the active mineral item, the ROWID of the item to edit is obtained from here
    update_columns hold abbreviations of columns to update
    to_update holds information that the columns specified in update_columns will be updated to
    */

    std::string sql, buffer; // sql stores sql query as it is constructed, buffer is a buffer for conversion of list-type items
    sqlite3_stmt *stmt; // database query object
    int rc = 0; // error code
    std::vector<std::string>::iterator it; // stores position of a given column in aliases
    int dist; // stores conversion of 'it' to an integer

    // list of possible column aliases that could appear in update_columns
    std::vector<std::string> aliases = {"name", "pagenum",    "category", "varieties", "structure", "formula",     "colours", "desc",        "images"};
    // column names corresponding to the aliases above
    std::vector<std::string> columns = {"Name", "PageNumber", "Category", "Varieties", "Structure", "Composition", "Colours", "Description", "Images"};

    sql = "UPDATE Minerals SET "; // first part of the UPDATE sql query

    for(unsigned int i = 0; i < update_columns.size(); i++) // for each column alias
    {
         it = std::find(aliases.begin(), aliases.end(), update_columns[i]); // find the position of the alias in aliases
         dist = std::distance(aliases.begin(), it); // convert this to an integer
         sql += columns[dist]; // add the column name corresponding to the alias given
         sql += "=?" + std::to_string(i+2); // set bind point. this starts at 2, as the ROWID will always occupy binding site ?1
         if(i != update_columns.size() - 1) // if alias is not the last in the list
            sql += ", "; // add a comma to separate this item from the next
    }

    // set update condition to the ROWID of the item to update
    // as the ROWID condition is the only bind site that is guaranteed to appear in any query, it is assigned binding site 1
    sql += " WHERE ROWID=?1";

    sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL); // prepare sql query

    sqlite3_bind_int(stmt, 1, active_item.id); // bind ROWID to WHERE condition

    for(unsigned int i = 0; i < update_columns.size(); i++) // for each alias
    {
        // for each if block, the current alias is checked
        // the appropriate value is then bound to the query
        // as aliases are being processed here in the same order as binding sites are added, we can simply bind each value as it appears to the i+2th site
        if(update_columns[i] == "name") // check value of alias
        {
            if(to_update.name == "~")
                sqlite3_bind_null(stmt, i+2);
            else                
                sqlite3_bind_text(stmt, i+2, to_update.name.c_str(), to_update.name.size(), SQLITE_STATIC); // bind value
        }
        else if(update_columns[i] == "pagenum")
        {
            if(to_update.pagenumber == -1)
                sqlite3_bind_null(stmt, i+2);
            else                
                sqlite3_bind_int(stmt, i+2, to_update.pagenumber);
        }
        else if(update_columns[i] == "category")
        {
            if(to_update.category == "~")
                sqlite3_bind_null(stmt, i+2);
            else                
                sqlite3_bind_text(stmt, i+2, to_update.category.c_str(), to_update.category.size(), SQLITE_STATIC);
        }
        else if(update_columns[i] == "structure")
        {
            if(to_update.structure == "~")
                sqlite3_bind_null(stmt, i+2);
            else                
                sqlite3_bind_text(stmt, i+2, to_update.structure.c_str(), to_update.structure.size(), SQLITE_STATIC);
        }
        else if(update_columns[i] == "formula")
        {
            if(to_update.composition== "~")
                sqlite3_bind_null(stmt, i+2);
            else                
                sqlite3_bind_text(stmt, i+2, to_update.composition.c_str(), to_update.composition.size(), SQLITE_STATIC);
        }
        else if(update_columns[i] == "desc")
        {
            if(to_update.description == "~")
                sqlite3_bind_null(stmt, i+2);
            else                
                sqlite3_bind_text(stmt, i+2, to_update.description.c_str(), to_update.description.size(), SQLITE_STATIC);
        }
        else if(update_columns[i] == "images")
        {
            if(to_update.images.size() == 0)
                sqlite3_bind_null(stmt, i+2);
            else                
            {
                for (unsigned int i = 0; i < to_update.images.size(); i++)
                {
                    if (namepart(to_update.images[i]) != to_update.images[i])
                    {
                        to_update.images[i] = assimilate_image(image_path, to_update.images[i]);
                    }
                }
                // for list type items, first convert the vector to a pipe separated string, as commas could appear in file names but pipes can't
                buffer = vec_to_sepstring(to_update.images); 
                // bind this to the query. use SQLITE_TRANSIENT as buffer will later be wiped, so sqlite needs to make its own copy
                sqlite3_bind_text(stmt, i+2, buffer.c_str(), buffer.size(), SQLITE_TRANSIENT); 
            }
        }
        else if(update_columns[i] == "varieties")
        {
            if(to_update.varieties.size() == 0)
                sqlite3_bind_null(stmt, i+2);
            else                
            {
                buffer = vec_to_str(to_update.varieties, true);
                sqlite3_bind_text(stmt, i+2, buffer.c_str(), buffer.size(), SQLITE_TRANSIENT);
            }
        }
        else if(update_columns[i] == "colours")
        {
            if(to_update.colours.size() == 0)
                sqlite3_bind_null(stmt, i+2);
            else                
            {
                buffer = vec_to_str(to_update.colours, true);
                sqlite3_bind_text(stmt, i+2, buffer.c_str(), buffer.size(), SQLITE_TRANSIENT);
            }
        }
    }

    rc = sqlite3_step(stmt); // execute sql query

    if(rc != SQLITE_DONE) // error
    {
        // IT'S ON FIRE! RUN FOR IT!!
        sqlite3_finalize(stmt);
        return sqlite3_errmsg(db); // returns the error that caused sqlite_step to stop
    }

    get_mineral_columns(active_item.id, {}, active_item); // update the active mineral item with the values just updated

    return 0;
}

const char * update_collection_item(collection_entry to_update, collection_entry &active_item, const std::string tablename, std::vector<std::string> update_columns, std::string image_path)
{
    /*
    this function updates columns of the active item in collection-type tables
    active_item is a reference to the active collection item, the ROWID of the item to edit is obtained from here
    update_columns hold abbreviations of columns to update
    to_update holds information that the columns specified in update_columns will be updated to
    */

    std::string sql, buffer; // sql stores sql query as it is constructed, buffer is a buffer for conversion of list-type items
    sqlite3_stmt *stmt; // database query object
    int rc = 0; // error code
    std::vector<std::string>::iterator it; // stores position of a given column in aliases
    int dist; // stores conversion of 'it' to an integer

    // list of possible column aliases that could appear in update_columns
    std::vector<std::string> aliases = {"num", "order", "name", "type", "desc", "colours", "date", "from", "notes", "cost", "buyer", "location", "texture", "images"};
    // column names corresponding to the aliases above
    std::vector<std::string> columns = {"PurchaseNumber","PurchaseNumberOrder","Name","MineralType","Description","Colours","Date","PurchaseLocation","Notes","Cost","Buyer","CurrentLocation","Texture","Images"};

    sql = "UPDATE "+tablename+" SET "; // first part of the UPDATE sql query

    for(unsigned int i = 0; i < update_columns.size(); i++) // for each column alias
    {
         it = std::find(aliases.begin(), aliases.end(), update_columns[i]); // find the position of the alias in aliases
         dist = std::distance(aliases.begin(), it); // convert this to an integer
         sql += columns[dist]; // add the column name corresponding to the alias given
         sql += "=?" + std::to_string(i+2); // set bind point. this starts at 2, as the ROWID will always occupy binding site ?1
         if(i != update_columns.size() - 1) // if alias is not the last in the list
            sql += ", "; // add a comma to separate this item from the next
    }

    // set update condition to the ROWID of the item to update
    // as the ROWID condition is the only bind site that is guaranteed to appear in any query, it is assigned binding site 1
    sql += " WHERE ROWID=?1";

    sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL); // prepare sql query

    sqlite3_bind_int(stmt, 1, active_item.id); // bind ROWID to WHERE condition

    for(unsigned int i = 0; i < update_columns.size(); i++) // for each alias
    {
        // for each if block, the current alias is checked
        // the appropriate value is then bound to the query
        // as aliases are being processed here in the same order as binding sites are added, we can simply bind each value as it appears to the i+2th site

        if(update_columns[i] == "order") // check value of alias
        {
            if(to_update.order == "~")
                sqlite3_bind_null(stmt, i+2);
            else
                sqlite3_bind_text(stmt, i+2, to_update.order.c_str(), to_update.order.size(), SQLITE_STATIC); // bind value
        }
        else if(update_columns[i] == "name")
        {
            if(to_update.name == "~")
                sqlite3_bind_null(stmt, i+2);
            else
                sqlite3_bind_text(stmt, i+2, to_update.name.c_str(), to_update.name.size(), SQLITE_STATIC);
        }
        else if(update_columns[i] == "desc")
        {
            if(to_update.description == "~")
                sqlite3_bind_null(stmt, i+2);
            else
                sqlite3_bind_text(stmt, i+2, to_update.description.c_str(), to_update.description.size(), SQLITE_STATIC);
        }
        else if(update_columns[i] == "date")
        {
            if(to_update.date == "~")
                sqlite3_bind_null(stmt, i+2);
            else
                sqlite3_bind_text(stmt, i+2, to_update.date.c_str(), to_update.date.size(), SQLITE_STATIC);
        }
        else if(update_columns[i] == "from")
        {
            if(to_update.buy_location == "~")
                sqlite3_bind_null(stmt, i+2);
            else
                sqlite3_bind_text(stmt, i+2, to_update.buy_location.c_str(), to_update.buy_location.size(), SQLITE_STATIC);
        }
        else if(update_columns[i] == "notes")
        {
            if(to_update.notes == "~")
                sqlite3_bind_null(stmt, i+2);
            else
                sqlite3_bind_text(stmt, i+2, to_update.notes.c_str(), to_update.notes.size(), SQLITE_STATIC);
        }
        else if(update_columns[i] == "buyer")
        {
            if(to_update.buyer == "~")
                sqlite3_bind_null(stmt, i+2);
            else
                sqlite3_bind_text(stmt, i+2, to_update.buyer.c_str(), to_update.buyer.size(), SQLITE_STATIC);
        }
        else if(update_columns[i] == "location")
        {
            if(to_update.now_location == "~")
                sqlite3_bind_null(stmt, i+2);
            else
                sqlite3_bind_text(stmt, i+2, to_update.now_location.c_str(), to_update.now_location.size(), SQLITE_STATIC);
        }
        else if(update_columns[i] == "num")
        {
            if(to_update.purchase == -1)
                sqlite3_bind_null(stmt, i+2);
            else
                sqlite3_bind_int(stmt, i+2, to_update.purchase);
        }
        else if(update_columns[i] == "cost")
        {
            if(to_update.cost == -1)
                sqlite3_bind_null(stmt, i+2);
            else
                sqlite3_bind_int(stmt, i+2, to_update.cost);
        }
        else if(update_columns[i] == "texture")
        {
            if(to_update.texture == -1)
                sqlite3_bind_null(stmt, i+2);
            else
                sqlite3_bind_int(stmt, i+2, to_update.texture);
        }
        else if(update_columns[i] == "images")
        {
            if(to_update.images.size() == 0)
                sqlite3_bind_null(stmt, i+2);
            else
            {
                for (unsigned int i = 0; i < to_update.images.size(); i++)
                {
                    if (namepart(to_update.images[i]) != to_update.images[i])
                    {
                        to_update.images[i] = assimilate_image(image_path, to_update.images[i]);
                    }
                }
                // for list type items, first convert the vector to a pipe separated string, as commas could appear in file names but pipes can't
                buffer = vec_to_sepstring(to_update.images, true);
                // bind this to the query. use SQLITE_TRANSIENT as buffer will later be wiped, so sqlite needs to make its own copy
                sqlite3_bind_text(stmt, i+2, buffer.c_str(), buffer.size(), SQLITE_TRANSIENT); 
            }
        }
        else if(update_columns[i] == "type")
        {
            if(to_update.mineral.size() == 0)
                sqlite3_bind_null(stmt, i+2);
            else
            {
                buffer = vec_to_str(to_update.mineral, true);
                sqlite3_bind_text(stmt, i+2, buffer.c_str(), buffer.size(), SQLITE_TRANSIENT);
            }
        }
        else if(update_columns[i] == "colours")
        {
            if(to_update.colours.size() == 0)
                sqlite3_bind_null(stmt, i+2);
            else
            {
                buffer = vec_to_str(to_update.colours, true);
                sqlite3_bind_text(stmt, i+2, buffer.c_str(), buffer.size(), SQLITE_TRANSIENT);
            }
        }
    }

    rc = sqlite3_step(stmt); // execute sql query

    if(rc != SQLITE_DONE) // error
    {
        // IT'S ON FIRE! RUN FOR IT!!
        sqlite3_finalize(stmt);
        return sqlite3_errmsg(db); // returns the error that caused sqlite_step to stop
    }

    get_collection_columns(active_item.id, {}, active_item, tablename); // update the active mineral item with the values just updated

    return 0;
}

int close_database()
{
    // safely closes the connection to the database
    int rc = sqlite3_close_v2(db);
    return rc;
}

const char * delete_row(const std::string table, int rowid)
{
    /*
    deletes the row with ROWID rowid from table
    if rowid equals -1, ALL records in the table are deleted!

    handle with care!
    */

    std::string sql; // sql query storage
    sqlite3_stmt *stmt; // sql query object
    int rc = 0; // error code object

    if(rowid == -1)
        sql = "DELETE FROM "+table; // sql query for deleting everything in a table
    else
    {
        sql = "DELETE FROM "+table+" WHERE ROWID=?1"; // sql query for deleting a row
    }

    sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL); // prepare the statement

    if(rowid != -1) // if rowid is not -1 - real ROWIDs are always greater than 1
        sqlite3_bind_int(stmt, 1, rowid); // bind rowid to sql statement

    while(rc != SQLITE_DONE)
    {
        rc = sqlite3_step(stmt); // execute statement
        if(rc == SQLITE_ERROR) // error
        {
            // IT'S ON FIRE! RUN FOR IT!!
            sqlite3_finalize(stmt);
            return sqlite3_errmsg(db); // returns the error that caused sqlite_step to stop
        }
    }

    // finalise the query
    sqlite3_finalize(stmt);
    return 0;
}

int change_table(program_config &config, std::string newtable)
{
    /*
    this function:
    1) commits the search results and sorting mask for the current table to memory in the config.alternate_table_info vector
    2) loads search results and sorting mask for the new table from memory
    3) changes the currently active table

    this is not needed for the Minerals table as it is unique
    */

    if(newtable == "Minerals")
    {
        // if the user is setting the table to Minerals there is no need to do anything but change active table
        config.table = "Minerals";
        config.active_item_table = "Minerals";
        return 0; // abort before the other stuff starts
    }

    bool found = false; // set to true if data is found in storage for newtable

    if(config.collection_results.tablename != "") // if collection_results actually hold any worthwhile data, add this to storage
    {
        config.alternate_table_info.push_back(config.collection_results);
    }

    for(unsigned int i = 0; i < config.alternate_table_info.size(); i++)
    { 
        // search through memory to find a search_result struct matching the new table
        if(config.alternate_table_info[i].tablename == newtable)
        {
            // set collection_results to the found data
            config.collection_results = config.alternate_table_info[i];
            // retrieve data about the active item for this table
            get_collection_columns(config.collection_results.active_item, {}, config.active_collection_item, newtable);

            // remove this data from memory
            config.alternate_table_info.erase(config.alternate_table_info.begin() + i);

            found = true;
        }
    }

    if(!found) // no data found, set defaults
    {
        config.collection_results.tablename = newtable; // new table
        config.collection_results.active_item = -1; // no active item
        config.collection_results.results = {}; // empty vector, no search results
        config.collection_results.sort_ascending = true; // sort ascending
        config.collection_results.sort_mask = 1; // sort by purchase number
        config.collection_results.search_or = false;
        config.collection_results.datecomp = 0;
        config.collection_results.costcomp = 0;
        config.collection_results.numcomp  = 0;
        collection_entry new_c;
        config.active_collection_item = new_c;
    }

    config.collection_table = newtable;
    config.table = config.collection_table; // set active table to the new table name
    config.active_item_table = config.table;

    return 0;
}

const char * get_tables(std::vector<std::string> & names)
{
    /*
    returns a vector containing the names of all tables
    */
    char *zErrMsg = 0; // pointer to error message
    int rc; // error code
    // this is where that hack neat the top of the file is used...
    table_type table; // creates an instance of the table_type type
    std::string sql; // sql string

    // the names of each table are stored in sqlite's master table, unsurprisingly called sqlite_master
    // retrieving all rows with the the type column is equal to table gets all tables
    sql = "SELECT name FROM sqlite_master WHERE type='table';";
     // execute sql statement, using the table_callback() function to populate table_type(table)
    rc = sqlite3_exec(db, sql.c_str(), table_callback, &table, &zErrMsg);
    names = table; // because table_type(table) is just a vector in disguise, I can simply set names to table and it all works nicely

    if(rc != SQLITE_OK)  // error
        return zErrMsg; // return pointer to error message
    return 0;
}

const char * make_table(std::string &tablename)
{
    /*
    creates a new empty collection-type table

    tables come in two types - collection-type and mineral-type
        the Minerals table is the only mineral-type table, this is automatically created and cannot be deleted
        it stores information about mineral types

        collection-type tables can be created and deleted at will
        they store information about a collection of stones
    */
    char *zErrMsg = 0; // pointer to error message
    int rc; // error code
    std::string sql;

    // censor tablename to remove all non-alphabetic characters
    // if censor() returns a non-zero value, there were no valid characters at all - ERROR
    if(censor(tablename)) return "Invalid table name"; // complain at the user

    // sql for creating a collection-type table
    // each line the is name of each column and the type of value that column stores
    // IF NOT EXISTS stops tables being overwritten
    sql = "CREATE TABLE IF NOT EXISTS "+tablename+" ("\
                "PurchaseNumber INTEGER,"\
                "PurchaseNumberOrder TEXT,"\
                "Name TEXT,"\
                "MineralType TEXT,"\
                "Description TEXT,"\
                "Colours TEXT,"\
                "Date TEXT,"\
                "PurchaseLocation TEXT,"\
                "Notes TEXT,"\
                "Cost INTEGER,"\
                "Buyer TEXT,"\
                "CurrentLocation TEXT,"\
                "Images TEXT,"\
                "Texture INTEGER);";

    // execute statement, with empty_callback() as nothing is returned
    rc = sqlite3_exec(db, sql.c_str(), empty_callback, 0, &zErrMsg);

    if(rc != SQLITE_OK) // error
        return zErrMsg;
    return 0;
}

const char * drop_table(program_config &config, const std::string tablename)
{
    /*
    deletes the table with name <tablename>, if it exists
    */
    char *zErrMsg = 0; // pointer to error message
    int rc; // error code
    std::string sql;

    // remove all data pertaining to this table from memory
    if(tablename == config.collection_table) // if the table to be deleted is the current collection-type table
    {
        if (config.collection_table_list.size() > 1)
        {
            for (std::string ctable : config.collection_table_list)
            {
                if (ctable != tablename)
                {
                    std::string temp_table = config.table;
                    change_table(config, ctable);
                    if(temp_table == "Minerals") 
                        change_table(config, temp_table);
                    break;
                }
            }
        }
        else
        {
            config.table = "Minerals";
            config.active_item_table = "Minerals";
            config.collection_table = "";

            search_result new_s;
            collection_entry new_c;

            config.active_collection_item = new_c; // reset active collection item
            config.collection_results = new_s; // reset this table's results
        }
    }
    else // information may be in memory as alternate item
    {
        for(unsigned int i = 0; i < config.alternate_table_info.size(); i++) // look through memory
        {
            if(config.alternate_table_info[i].tablename == tablename) // if information for table is in memory
            {
                // remove this information
                config.alternate_table_info.erase(config.alternate_table_info.begin() + i);
                i--;
            }
        }
    }

    std::vector<std::string>::iterator it = std::find(config.collection_table_list.begin(), config.collection_table_list.end(), tablename);
    if (it != config.collection_table_list.end())
    {
        config.collection_table_list.erase(it);
    }

    sql = "DROP TABLE IF EXISTS "+tablename+";"; // sql command for deleting a table

    rc = sqlite3_exec(db, sql.c_str(), empty_callback, 0, &zErrMsg); // execute

    if(rc != SQLITE_OK) // error
        return zErrMsg;
    return 0;
}

int change_table(program_config &config, const char * newtable)
{
    return change_table(config, std::string(newtable));
}

void backup_database(int type, const std::string apppath, const std::string & alternate_source = "")
{
    /*
    creates a backup copy of Stones.db
    if type is 0, the file created is Stones.db.boot
    if type is 1, the file created is Stones.db.close
    */

    std::ifstream src;
    std::ofstream dst;

    if(type == 0)
        dst.open(apppath+"Stones.db.boot", std::ios::binary);
    else if(type == 1)
        dst.open(apppath+"Stones.db.close", std::ios::binary);

   if(alternate_source != "")
   {
       src.open(apppath+alternate_source, std::ios::binary);
   }
   else
   {
       src.open(apppath+"Stones.db", std::ios::binary);
   }

    dst << src.rdbuf();
}

bool compare_files(std::string filepath, const char * file_one, const char * file_two)
{
    /*
    compares two files as efficiently as possible
    returns true if they are the same and false otherwise
    */

    std::string first = filepath + file_one;
    std::string second = filepath + file_two;

    // if the file size is different they must be different
    if (filesize(first.c_str()) != filesize(second.c_str()))
    {
        return false;
    }
    std::ifstream::streampos nulli = 0;
    if ((filesize(first.c_str()) == nulli) || ( filesize(second.c_str()) == nulli))
    {
        return false;
    }
    
    // open filestreams for the two files
    std::ifstream lFile(first, std::ios::in | std::ios::binary);
    std::ifstream rFile(second, std::ios::in | std::ios::binary);

    // if there has been a problem opening the files 
    if(!lFile.is_open() || !rFile.is_open())
    {
        return false;
    }

    // create 128 byte read buffers
    char *lBuffer = new char[1024]();
    char *rBuffer = new char[1024]();

    do
    {
        // read 128 bytes from each file
        lFile.read(lBuffer, 1024);
        rFile.read(rBuffer, 1024);
        // get the number of characters actually read
        auto numberOfRead = lFile.gcount();

        if (std::memcmp(lBuffer, rBuffer, (size_t)numberOfRead) != 0) // if memory blocks are NOT equal
        {
            memset(lBuffer,0,(size_t)numberOfRead);
            memset(rBuffer,0,(size_t)numberOfRead);
            return false;
        }
    } while (lFile.good() || rFile.good()); // while file data remains

    // deallocate buffers
    delete[] lBuffer;
    delete[] rBuffer;
    return true;
}