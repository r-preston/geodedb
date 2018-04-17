#ifndef DATA_STRUCTURE_MINERALS_HEADER
#define DATA_STRUCTURE_MINERALS_HEADER

/*
The Nickel-Strunz mineral classification system:

 1.  Elements
 2.  Sulphides
 3.  Halides
 4.  Oxides
 5.  Carbonates
 6.  Borates
 7.  Sulphates
 8.  Phosphates
 9.  Silicates
 10. Organic compounds
 11. Rocks, meteorites and tektites (not official, but included in Dad's mineral book)
*/

// can store all data relating to a collection item
struct collection_entry
{
    int id; // internal sqlite ROWID of item
    int purchase; // number of purchase
    int cost; // cost in pence
    int texture; // 1 means tumbled, 0 means rough
    std::string order; // order in purchase
    std::string name; // name of mineral
    std::string description; // description of physical appearance
    std::string date; // date of purchase
    std::string buyer; // who bought it
    std::string notes; // miscellaneous information - 'bought for nth birthday' etc.
    std::string buy_location; // where was it bought
    std::string now_location; // where does it live now - usually Bowl or Cabinet
    std::vector<std::string> mineral; // list of minerals composing the stone
    std::vector<std::string> images; // list of image paths
    std::vector<std::string> colours; // list of colour present in the stone
    collection_entry()
    {
        // initialises all integer values to -1, which indicates a NULL value
        id = -1;
        purchase = -1;
        cost = -1;
        texture = -1;
    }
};

// can store all data relating to a mineral type
struct mineral_entry
{
    int id; // internal sqlite ROWID of item
    int pagenumber; // page number of mineral in Dad's mineral encyclopaedia - 'The Complete Encyclopedia of Minerals', Petr Korbel and Milan Nov�k (1999, Rebo)
    std::string category; // Nickel-Strunz category - see above
    std::string name; // name of mineral type
    std::string description; // description of mineral type
    std::string structure; // crystal structure - monoclinic etc
    std::string composition; // chemical formula
    std::vector<std::string> varieties; // list of names of varieties
    std::vector<std::string> colours; // list of common colours
    std::vector<std::string> images; // list of image paths
    mineral_entry()
    {
        // initialises all integer values to -1, which indicates a NULL value
        id = -1;
        pagenumber = -1;
    }
};

// a simple structure for search results
struct search_result
{
    std::string tablename; // table the results came from
    std::vector<int> results; // stores a list of matched rowids
    int active_item; // the currently visible item for this table

    int sort_mask; // sorting mask - 0=name, 1=purchase number, 2=date, 3=cost, 4=mineral type. only name applies to the Minerals table
    bool sort_ascending; // true to sort ascending, false to sort descending
    bool search_or; // false = AND, true = OR when searching

    // greater than, less than, equal to specifiers for searching
    // not applicable to the Minerals table
    int datecomp;
    int costcomp;
    int numcomp;

    search_result(); // constructor function, called when object is created
};

// stores program configuration, read from config file at startup
struct program_config
{
    int config_loaded; // 0 if load() has not been run. designed to stop config being overwritten if the program is closed before config is loaded
    bool is_new; // true if the item about to be saved is new, rather than an edited existing item
    bool is_editing; // true if the item editing window is open
    bool has_closed; // true when program_config::shutdown has completed. prevents multiple shutdown processes

    std::string config_path;
    std::string db_path;
    std::string image_path;

    std::string table; // current active table
    std::string collection_table; // current collection table
    std::vector<std::string> collection_table_list; // list of collection tables
    std::string active_item_table; // table that the currently visible item belongs to

    search_result collection_results; // structure containing search results and sorting mask for active collection-type table
    search_result minerals_results; // structure containing search results and sorting mask Minerals table

    collection_entry active_collection_item; // active collection item
    mineral_entry active_mineral_item; // active mineral item

     // for collection-type tables, many tables could be sharing one object. this vector is memory for tables that are not currently active
    std::vector<search_result> alternate_table_info;

    // index of current image in active item
    int collection_current_image;
    int mineral_current_image;

    std::vector<std::string> visible_collection_columns; // names of all visible columns in collection result list
    std::vector<std::string> collection_column_widths; // widths of the above columns

    std::vector<std::string> visible_mineral_columns; // names of all visible columns in mineral result list
    std::vector<std::string> mineral_column_widths; // widths of the above columns

    int save(bool force = false); // saves the contents of this object
    int load(); // loads the contents of this object
    int shutdown(); // closes the database, backs up database and returns the value of this.save()

    program_config(); // constructor function, called when object is created
    ~program_config(); // destructor function, called when object is destroyed or goes out of scope
};

#endif
