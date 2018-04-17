function empty_return(str) { return str; }

function seperate_commas(str) { return explode(str, ","); }

function seperate_images(str) { return explode(str, "\x1c"); }

function to_integer(str) { return parseInt(str, 10); }

function to_boolean(str) { return (str === "1"); }

function to_integer_list(str)
{
    var values = explode(str, ",");
    for(var i = 0; i < values.length; i++)
    {
        values[i] = parseInt(values[i], 10);
    }
}

function apply_synced_config(data, mode)
{
    // program_config.alternate_table_info isn't actually used on the JS side, so isn't synced
    var conversion = ["empty_return", "seperate_commas", "to_integer", "to_integer_list", "seperate_images", "to_boolean"];
    var item = 0;
    var properties = [];
    var conversion_code = [];
    var second_level = "";
    var temp = "";

    if(mode === "program_config")
    {
        properties = ["table", "collection_table", "collection_table_list", "active_item_table",
                      "collection_current_image", "mineral_current_image", "visible_collection_columns",
                      "collection_column_widths", "visible_mineral_columns", "mineral_column_widths",
                      "image_path"];
        // 0 = nothing, 1 = list of strings, 2 = int, 3 = list of ints, 4 = list of images
        conversion_code = [0, 0, 1, 0, 2, 2, 1, 1, 1, 1, 0];
    }
    else if(mode === "program_config.collection_results")
    {
        properties = ["tablename", "results", "active_item", "sort_mask",
                      "sort_ascending", "search_or", "datecomp", "costcomp", "numcomp"];
        second_level = "collection_results";
        // 0 = nothing, 1 = list of strings, 2 = int, 3 = list of ints, 4 = list of images, 5 = boolean
        conversion_code = [0, 3, 2, 2, 5, 5, 2, 2, 2];
    }
    else if(mode === "program_config.minerals_results")
    {
        properties = ["tablename", "results", "active_item", "sort_mask", "sort_ascending", "search_or"];
        second_level = "minerals_results";
        // 0 = nothing, 1 = list of strings, 2 = int, 3 = list of ints, 4 = list of images
        conversion_code = [0, 3, 2, 2, 5, 5];
    }
    else if(mode === "program_config.active_collection_item")
    {
        properties = ["rowid", "number", "cost", "texture", "order", "name", "description", "date",
                      "buyer", "notes", "purchaseplace", "location", "type", "images", "colours"];
        second_level = "active_collection_item";
        // 0 = nothing, 1 = list of strings, 2 = int, 3 = list of ints, 4 = list of images
        conversion_code = [2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 1];
    }
    else if(mode === "program_config.active_mineral_item")
    {
        properties = ["rowid", "pagenumber", "category", "name", "description", "structure",
                      "composition", "varieties", "colours", "images"];
        second_level = "active_mineral_item";
        // 0 = nothing, 1 = list of strings, 2 = int, 3 = list of ints, 4 = list of images
        conversion_code = [2, 2, 0, 0, 0, 0, 0, 1, 1, 4];
    }

    for(var i = 0; i < data.length; i++)
    {
        if(String(data[i]) === String.fromCharCode(0x1E))
        {
            if(second_level === "")
            {
                program_config[properties[item]] = window[conversion[conversion_code[item]]](temp);
            }
            else
            {
                program_config[second_level][properties[item]] = window[conversion[conversion_code[item]]](temp);
            }
            temp = '';
            item++;
        }
        else
        {
            temp += data[i];
        }
    }
    if(second_level === "")
    {
        program_config[properties[item]] = window[conversion[conversion_code[item]]](temp);
    }
    else
    {
        program_config[second_level][properties[item]] = window[conversion[conversion_code[item]]](temp);
    }
}

function apply_synced_config_legacy(data, mode)
{
    switch(mode)
    {
        case "program_config":
            program_config.table = data[0];
            program_config.collection_table = data[1];
            program_config.collection_table_list = data[2];
            program_config.active_item_table = data[3];
            program_config.collection_current_image = data[4];
            program_config.mineral_current_image = data[5];
            program_config.visible_collection_columns = data[6];
            program_config.collection_column_widths = data[7];
            program_config.visible_mineral_columns = data[8];
            program_config.mineral_column_widths = data[9];
            program_config.image_path = data[10];
            break;
        case "program_config.collection_results":
            program_config.collection_results.tablename = data[0];
            program_config.collection_results.results = data[1];
            program_config.collection_results.active_item = data[2];
            program_config.collection_results.sort_mask = data[3];
            program_config.collection_results.sort_ascending = data[4];
            program_config.collection_results.search_or = data[5];
            program_config.collection_results.datecomp = data[6];
            program_config.collection_results.costcomp = data[7];
            program_config.collection_results.numcomp = data[8];
            break;
        case "program_config.minerals_results":
            program_config.minerals_results.tablename = data[0];
            program_config.minerals_results.results = data[1];
            program_config.minerals_results.active_item = data[2];
            program_config.minerals_results.sort_mask = data[3];
            program_config.minerals_results.sort_ascending = data[4];
            program_config.minerals_results.search_or = data[5];
            break;
        case "program_config.active_collection_item":
            program_config.active_collection_item.rowid = data[0];
            program_config.active_collection_item.number = data[1];
            program_config.active_collection_item.cost = data[2];
            program_config.active_collection_item.texture = data[3];
            program_config.active_collection_item.order = data[4];
            program_config.active_collection_item.name = data[5];
            program_config.active_collection_item.description = data[6];
            program_config.active_collection_item.date = data[7];
            program_config.active_collection_item.buyer = data[8];
            program_config.active_collection_item.notes = data[9];
            program_config.active_collection_item.purchaseplace = data[10];
            program_config.active_collection_item.location = data[11];
            program_config.active_collection_item.type = data[12];
            program_config.active_collection_item.images = data[13];
            program_config.active_collection_item.colours = data[14];
            break;
        case "program_config.active_mineral_item":
            program_config.active_mineral_item.rowid = data[0];
            program_config.active_mineral_item.pagenumber = data[1];
            program_config.active_mineral_item.category = data[2];
            program_config.active_mineral_item.name = data[3];
            program_config.active_mineral_item.description = data[4];
            program_config.active_mineral_item.structure = data[5];
            program_config.active_mineral_item.composition = data[6];
            program_config.active_mineral_item.varieties = data[7];
            program_config.active_mineral_item.colours = data[8];
            program_config.active_mineral_item.images = data[9];
            break;
    }
}
