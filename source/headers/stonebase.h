#ifndef STONEBASE_DB_ACCESS_HEADER
#define STONEBASE_DB_ACCESS_HEADER

struct collection_entry;
struct mineral_entry;

// update an existing row, editing the columns specified in update_columns
const char * update_mineral_item(mineral_entry to_update, mineral_entry &active_item, std::vector<std::string> update_columns, std::string image_path);

// update an existing row in tablename, editing the columns specified in update_columns
const char * update_collection_item(collection_entry to_update, collection_entry &active_item, const std::string tablename, std::vector<std::string> update_columns, std::string image_path);

// deletes the row with ROWID equal to rowid in given table
const char * delete_row(const std::string table, int rowid);

// checks if an item exists in a table, id is set to the ROWID of the item if found, otherwise set to -1
const char * does_exist(const std::string table, int num, std::string matchstring, int &id);

// changes active table and sets remembered search results accordingly
int change_table(program_config &config, std::string newtable);
int change_table(program_config &config, const char * newtable);

// sorts rowids based on sort_mask
const char * sort_items(std::vector<int> &rowids, std::string table, bool ascend, int sort_mask = 0);

// retrieves column data items from current collection search results
const char * get_collection_columns(int rowid, std::vector<std::string> columns, collection_entry &data, std::string tablename);

// retrieves column data items from current collection search results
const char * get_mineral_columns(int rowid, std::vector<std::string> columns, mineral_entry &data);

// custom date search for search_collection
const char * search_date(std::vector<int> &results, const std::string tablename, std::string datestring, int comp, bool andor);

// search a collection-type table
const char * search_collection(collection_entry args, std::string any, int argnum, const std::string tabnam, search_result &matched, std::vector<int> compares = {0,0,0}, bool s_or = true);

// search the table 'Minerals'
const char * search_minerals(mineral_entry args, std::string any, int argnum, search_result &matched, bool s_or = true);

// open Stones.db and establish database connection
const char * open_database(std::string image_path);

// obtains the number of rows in a table
const char * get_table_records(std::string tablename, int &counts);

// closes connection, deallocates memory and closes the database safely
int close_database();

// create a new row in a collection-type table
const char * new_collection_row(collection_entry &new_entry, const std::string tablename, const std::string image_path);

// creates a new row in Minerals
const char * new_mineral_row(mineral_entry &new_entry, const std::string image_path);

// gets the name of all currently existing tables
const char * get_tables(std::vector<std::string> & names);

// create a new table
const char * make_table(std::string &tablename);

// delete a table
const char * drop_table(program_config &config, const std::string tablename);

// creates a copy of Stones.db. 'type' defines the name of the copy
void backup_database(int type, const std::string apppath, const std::string & alternate_source = "");

// compares a file to Stones.db
bool compare_files(std::string apppath, const char * file_one, const char * file_two);

#endif
