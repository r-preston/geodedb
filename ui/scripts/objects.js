function CollectionEntry()
{
    this.rowid = -1;
    this.number = -1;
    this.order = '';
    this.name = '';
    this.type = [];
    this.description = '';
    this.colours = [];
    this.date = '';
    this.purchaseplace = '';
    this.notes = '';
    this.cost = -1;
    this.buyer = '';
    this.location = '';
    this.images = [];
    this.texture = -1;
}

function MineralEntry()
{
    this.rowid = -1;
    this.name = '';
    this.varieties = [];
    this.description = '';
    this.colours = [];
    this.structure = '';
    this.composition = '';
    this.images = [];
    this.category = '';
    this.pagenumber = -1;
}

function SearchResult()
{
    this.tablename = '';
    this.results = [];
    this.active_item = -1;
    this.sort_mask = 0; // 0=name, 1=purchase number, 2=date, 3=cost, 4=mineral type. for the Minerals table, only name applies
    this.sort_ascending = true;
    this.search_or = false; // false = AND, true = OR when searching
    this.datecomp = 0; // not applicable to Minerals table
    this.costcomp = 0; // ditto
    this.numcomp  = 0; // ditto
}

/*function ProgramConfig()
{
    this.table = '';
    this.collection_table = '';
    this.collection_table_list = [];
    this.active_item_table = '';

    this.collection_results = new SearchResult();
    this.minerals_results = new SearchResult();
    this.active_collection_item = new CollectionEntry();
    this.active_mineral_item = new MineralEntry();
    this.alternate_table_info = [];

    this.collection_current_image = -1;
    this.mineral_current_image = -1;

    this.visible_collection_columns = [];
    this.collection_column_widths = [];

    this.visible_mineral_columns = [];
    this.mineral_column_widths = [];
}
*/
