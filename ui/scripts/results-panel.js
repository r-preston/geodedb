function format_data(raw, col)
{
    var list_types = ["colours", "varieties", "type"];
    var pos = list_types.indexOf(col);

    if(raw === -1) return '';

    if(pos !== -1)
    {
        return display_list(raw);
    }
    if(col === "images")
    {
        return raw.length;
    }
    if(col === "date")
    {
        if(raw === '') return raw;
        return full_date(raw, true);
    }
    if(col === "composition")
    {
        return subscript_numbers(raw);
    }
    if(col === "cost")
    {
        return pence_to_pounds(raw);
    }
    if(col === "texture")
    {
        if(raw === 1) return "Tumbled";
        if(raw === 0) return "Rough";
    }
    return raw;
}

function add_mineral_result(mineral_data, position)
{
    position = (typeof position !== 'undefined') ? position : 0;

    var external = false;
    mineral_data_list = [];
    var columnlist = ["name", "varieties", "description", "colours", "structure", "composition", "images", "category", "pagenumber"];
    //if(mineral_data_list.length === 10)
    if(typeof mineral_data === "string")
    {
        // 0 = nothing, 1 = list of strings, 2 = int, 3 = list of ints, 4 = list of images
        conversion_code = [2, 0, 1, 0, 1, 0, 0, 4, 0, 2];
        var conversion = ["empty_return", "seperate_commas", "to_integer", "to_integer_list", "seperate_images", "to_boolean"];
        var item = 0;
        var temp = "";

        for(var i = 0; i < mineral_data.length; i++)
        {
            if(String(mineral_data[i]) === String.fromCharCode(0x1E))
            {
                mineral_data_list.push( window[conversion[conversion_code[item]]](temp) );
                temp = '';
                item++;
            }
            else
            {
                temp += mineral_data[i];
            }
        }
        mineral_data_list.push( window[conversion[conversion_code[item]]](temp) );
        external = true;
    }

    var newline = document.createElement("div");
    newline.classList.add("result-line", "mineral-result");

    if(external)
    {
        newline.setAttribute("rowid", mineral_data_list[0]);
    }
    else
    {
        newline.setAttribute("rowid", mineral_data_list.rowid);
    }

    var newcol, tnode, nesttable, nesttr, nesttd;

    for(var i = 0; i < columnlist.length; i++)
    {
        //ui.printmsg("'"+columnlist[i]+"' :: "+mineral_data_list[ i + 1 ] );
        newcol = document.createElement("div");
        newcol.classList.add("mres-"+columnlist[i]);
        newcol.classList.add("mcol-"+columnlist[i]);

        tnode = document.createTextNode("");
        nesttable = document.createElement("table");
        nesttr = document.createElement("tr");
        nesttd = document.createElement("td");

        nesttable.classList.add("result-cell-wrapper");
        nesttd.classList.add("result-cell-text");

        nesttd.appendChild(tnode);
        nesttr.appendChild(nesttd);
        nesttable.appendChild(nesttr);
        newcol.appendChild(nesttable);

        if(external)
        {
            nesttd.innerHTML = format_data(mineral_data_list[ i + 1 ], columnlist[i] );

        }
        else
        {
            nesttd.innerHTML = format_data(mineral_data_list[ columnlist[i] ], columnlist[i] );
        }

        newcol.style.width = document.getElementById("mlab-"+columnlist[i]).style.width;

        var query_columns = document.getElementsByClassName("sorting-tab mcol-"+columnlist[i]);

        if(query_columns[0].classList.contains("show-column"))
        {
            newcol.classList.add("show-column");
        }

        newline.appendChild(newcol);
    }

    newline.onclick = function(){ui.activate_mineral_item(this.getAttribute("rowid"))};

    if(position % 2 === 1)
    {
         newline.classList.add("result-grey-background");
    }

    document.getElementById("mineral-result-line-container").appendChild(newline);
}

function add_collection_result(collection_data, position)
{
    position = (typeof position !== 'undefined') ?  position : 0;

    var external = false;
    collection_data_list = [];
    var columnlist = ["number", "order", "name", "type", "description", "colours", "date", "purchaseplace", "notes", "cost", "buyer", "location", "images", "texture"];
    //if(collection_data.length === 15)
    if(typeof collection_data === "string")
    {
        // 0 = nothing, 1 = list of strings, 2 = int, 3 = list of ints, 4 = list of images
        conversion_code = [2, 2, 0, 0, 1, 0, 1, 0, 0, 0, 2, 0, 0, 4, 2];
        var conversion = ["empty_return", "seperate_commas", "to_integer", "to_integer_list", "seperate_images", "to_boolean"];
        var item = 0;
        var temp = "";

        for(var i = 0; i < collection_data.length; i++)
        {
            if(String(collection_data[i]) === String.fromCharCode(0x1E))
            {
                collection_data_list.push( window[conversion[conversion_code[item]]](temp) );
                temp = '';
                item++;
            }
            else
            {
                temp += collection_data[i];
            }
        }
        collection_data_list.push( window[conversion[conversion_code[item]]](temp) );
        external = true;
    }

    var newline = document.createElement("div");
    newline.classList.add("result-line", "collection-result");

    if(external)
    {
        newline.setAttribute("rowid", collection_data_list[0]);
    }
    else
    {
        newline.setAttribute("rowid", collection_data.rowid);
    }

    var newcol, tnode, nesttable, nesttr, nesttd;
    for(var i = 0; i < columnlist.length; i++)
    {
        //ui.printmsg("'"+columnlist[i]+"' :: "+collection_data_list[ i + 1 ] );
        newcol = document.createElement("div");

        newcol.classList.add("cres-"+columnlist[i]);
        newcol.classList.add("ccol-"+columnlist[i]);

        tnode = document.createTextNode("");
        nesttable = document.createElement("table");
        nesttr = document.createElement("tr");
        nesttd = document.createElement("td");

        nesttable.classList.add("result-cell-wrapper");
        nesttd.classList.add("result-cell-text");

        nesttd.appendChild(tnode);
        nesttr.appendChild(nesttd);
        nesttable.appendChild(nesttr);
        newcol.appendChild(nesttable);

        if(external)
        {
            nesttd.innerHTML = format_data( collection_data_list[ i + 1 ], columnlist[i] );
        }
        else
        {
            nesttd.innerHTML = format_data( collection_data_list[ columnlist[i] ], columnlist[i] );
        }

        newcol.style.width = document.getElementById("clab-"+columnlist[i]).style.width;

        var query_columns = document.getElementsByClassName("sorting-tab ccol-"+columnlist[i])
        if(query_columns[0].classList.contains("show-column"))
        {
            newcol.classList.add("show-column");
        }

        newline.appendChild(newcol);
    }

    newline.onclick = function(){ui.activate_collection_item(this.getAttribute("rowid"))};

    if(position % 2 === 1)
    {
         newline.classList.add("result-grey-background");
    }

    document.getElementById("collection-result-line-container").appendChild(newline);
}

function show_collection_context(x, y)
{
    var context_menu = document.getElementById('rclick-context-collection');
    if(!context_menu.classList.contains("show-checkbox"))
    {
        context_menu.classList.add("show-checkbox");
    }
    context_menu.style.top = y + 'px';
    context_menu.style.left = x + 'px';
}

function show_mineral_context(x, y)
{
    var context_menu = document.getElementById('rclick-context-mineral');
    if(!context_menu.classList.contains("show-checkbox"))
    {
        context_menu.classList.add("show-checkbox");
    }
    context_menu.style.top = y + 'px';
    context_menu.style.left = x + 'px';
}

function add_context_listeners()
{
    document.getElementById("collection-results-topbar").addEventListener('contextmenu', function(event)
                                                                                         {
                                                                                             event.preventDefault();
                                                                                             show_collection_context(event.pageX, event.pageY);
                                                                                             return false;
                                                                                         }, false);
    document.getElementById("mineral-results-topbar").addEventListener('contextmenu', function(event)
                                                                                         {
                                                                                             event.preventDefault();
                                                                                             show_mineral_context(event.pageX, event.pageY);
                                                                                             return false;
                                                                                         }, false);
    return 0;
}

function remove_collection_results()
{
    var container = document.getElementById("collection-result-line-container");
    while (container.firstChild)
    {
        container.removeChild(container.firstChild);
    }
}

function remove_mineral_results()
{
    var container = document.getElementById("mineral-result-line-container");
    while (container.firstChild)
    {
        container.removeChild(container.firstChild);
    }
}

function total_length(toResize)
{
    var total = 0;
    for(var i = 0; i < toResize.length; i++)
    {
        total += toResize[i].offsetWidth;
    }
    return total;
}

function toggle_mineral_column(checkid)
{
    visible_mineral_columns = program_config.visible_mineral_columns;
    mineral_column_widths = program_config.mineral_column_widths;

    checkid = "mcol-" + checkid.substring(7);
    columnid = checkid.substring(5);

    var colcells = document.getElementsByClassName(checkid);

    var create = true;
    if(colcells[0].classList.contains("show-column"))
    {
        create = false;
    }

    var toResize = [];
    var tooSmall = [];
    var shed_width = 0;
    var colList = document.getElementById("mineral-results-topbar").children;

    for(var i = 0; i < colList.length; i++)
    {
        if(colList[i].classList.contains("show-column") && (colList[i].id !== "mlab-"+columnid))
        {
            toResize.push(colList[i]);
            if(colList[i].offsetWidth < 23)
            {
                tooSmall.push(colList[i]);
                shed_width += colList[i].offsetWidth - 20;
            }
        }
    }

    var totalwidth = total_length(toResize);
    var z = total_length(tooSmall);
    var barwidth = document.getElementById("mineral-results-topbar").offsetWidth;
    var newWidth = 0;

    if(create)
    {
        for(var i = 0; i < toResize.length; i++)
        {
            if(toResize[i].offsetWidth > 22)
            {
                newWidth = toResize[i].offsetWidth * ( ( ( 0.87 * barwidth ) - z + shed_width ) / ( totalwidth - z ) ); // in px
                newWidth = Math.floor(newWidth + 0.5);
            }
            else
            {
                newWidth = 20; // in px
            }
            mineral_column_widths = set_mineral_column_width( toResize[i].id.substr(5), (newWidth * 100 / barwidth).toString() + '%', barwidth, visible_mineral_columns);
        }
        if(visible_mineral_columns.indexOf(columnid) === -1)
        {
            visible_mineral_columns = push_to_list("visible_mineral_columns", columnid);
            mineral_column_widths = push_to_list("mineral_column_widths", "0%");
        }
        mineral_column_widths = set_mineral_column_width( checkid.substr(5), '13%', barwidth, visible_mineral_columns);
    }
    else
    {
        for(var i = 0; i < toResize.length; i++)
        {
            newWidth = ( toResize[i].offsetWidth * barwidth ) / totalwidth; // in px
            newWidth = Math.floor(newWidth + 0.5);
            mineral_column_widths = set_mineral_column_width( toResize[i].id.substr(5), (100 * newWidth / barwidth).toString() + '%', barwidth, visible_mineral_columns);
        }
    }

    for(var i = 0; i < colcells.length; i++)
    {
        colcells[i].classList.toggle("show-column");
    }
}

function toggle_collection_column(checkid)
{
    visible_collection_columns = program_config.visible_collection_columns;
    collection_column_widths = program_config.collection_column_widths;

    checkid = "ccol-" + checkid.substring(7);
    columnid = checkid.substring(5);

    var colcells = document.getElementsByClassName(checkid);

    var create = true;
    if(colcells[0].classList.contains("show-column"))
    {
        create = false;
    }

    var toResize = [];
    var tooSmall = [];
    var shed_width = 0;
    var colList = document.getElementById("collection-results-topbar").children;

    for(var i = 0; i < colList.length; i++)
    {
        if(colList[i].classList.contains("show-column") && (colList[i].id !== "clab-"+columnid))
        {
            toResize.push(colList[i]);
            if(colList[i].offsetWidth < 23)
            {
                tooSmall.push(colList[i]);
                shed_width += colList[i].offsetWidth - 20;
            }
        }
    }

    var totalwidth = total_length(toResize);
    var z = total_length(tooSmall);
    var barwidth = document.getElementById("collection-results-topbar").offsetWidth;
    var newWidth = 0;

    if(create)
    {
        for(var i = 0; i < toResize.length; i++)
        {
            if(toResize[i].offsetWidth > 22)
            {
                newWidth = toResize[i].offsetWidth * ( ( ( 0.87 * barwidth ) - z + shed_width ) / ( totalwidth - z ) ); // in px
                newWidth = Math.floor(newWidth + 0.5);
            }
            else
            {
                newWidth = 20; // in px
            }
            collection_column_widths = set_collection_column_width( toResize[i].id.substr(5), (newWidth * 100 / barwidth).toString() + '%', barwidth, visible_collection_columns);
        }
        if(visible_collection_columns.indexOf(columnid) === -1)
        {
            visible_collection_columns = push_to_list("visible_collection_columns", columnid);
            collection_column_widths = push_to_list("collection_column_widths", "0%");
        }
        collection_column_widths = set_collection_column_width( checkid.substr(5), '13%', barwidth, visible_collection_columns);
    }
    else
    {
        for(var i = 0; i < toResize.length; i++)
        {
            newWidth = ( toResize[i].offsetWidth * barwidth ) / totalwidth; // in px
            newWidth = Math.floor(newWidth + 0.5);
            collection_column_widths = set_collection_column_width( toResize[i].id.substr(5), (100 * newWidth / barwidth).toString() + '%', barwidth, visible_collection_columns);
        }
    }

    for(var i = 0; i < colcells.length; i++)
    {
        colcells[i].classList.toggle("show-column");
    }
}

function activate_mineral_column(col_id)
{
    var identifier = "mcheck-" + col_id;
    document.getElementById(identifier).checked = true;
    toggle_mineral_column(identifier);
}

function activate_collection_column(col_id)
{
    var identifier = "ccheck-" + col_id;
    document.getElementById(identifier).checked = true;
    toggle_collection_column(identifier);
}

function set_mineral_column_width(col_id, x, barwidth, visible_mineral_columns, percentages)
{
    percentages = (typeof percentages !== 'undefined') ?  percentages : true;
    if((x.substr(-2,2) === "px") && (percentages === true))
    {
        x = parseInt(x + 1)*(100/barwidth) + "%";
    }

    var index = visible_mineral_columns.indexOf(col_id);
    var mineral_column_widths = modify_list("mineral_column_widths", index, x);

    col_id = "mcol-" + col_id;
    var column = document.getElementsByClassName(col_id);
    for(var i = 0; i < column.length; i++)
    {
        column[i].style.width = x;
        if(i === 1)
        {
            if(column[0].offsetWidth !== column[1].offsetWidth)
            {
                x = column[0].offsetWidth.toString() + 'px';
                column[1].style.width = x;
            }
        }
    }
    return mineral_column_widths;
}

function set_collection_column_width(col_id, x, barwidth, visible_collection_columns, percentages)
{
    percentages = (typeof percentages !== 'undefined') ?  percentages : true;
    if((x.substr(-2,2) === "px") && (percentages === true))
    {
        x = parseInt(x)*(100/barwidth) + "%";
    }

    var index = visible_collection_columns.indexOf(col_id);
    var collection_column_widths = modify_list("collection_column_widths", index, x);

    col_id = "ccol-" + col_id;
    var column = document.getElementsByClassName(col_id);
    for(var i = 0; i < column.length; i++)
    {
        column[i].style.width = x;
        if(i === 1)
        {
            if(column[0].offsetWidth !== column[1].offsetWidth)
            {
                x = column[0].offsetWidth.toString() + 'px';
                column[1].style.width = x;
            }
        }
    }
    return collection_column_widths;
}

function change_mineral_sorting_mask()
{
    var indicator = document.getElementById("m-name-sorting-arrow");

    var temp = !(program_config.minerals_results.sort_ascending);
    program_config.minerals_results.sort_ascending = temp;

    if( temp === true )
    {
        indicator.src = "resource/arrow_down.png";
    }
    else
    {
        indicator.src = "resource/arrow_up.png";
    }
    var resultlines = document.getElementById("mineral-result-line-container").children;
    for(var i = 0; i < resultlines.length; i++)
    {
        if(i % 2 === 1)
        {
             resultlines[i].classList.add("result-grey-background");
        }
        else
        {
             resultlines[i].classList.remove("result-grey-background");
        }
    }
}

function change_collection_sorting_mask(mask)
{
    var sort_ascending = program_config.collection_results.sort_ascending;
    var sort_mask = program_config.collection_results.sort_mask;

    if(mask === sort_mask)
    {
        sort_ascending = !(sort_ascending);
        program_config.collection_results.sort_ascending = sort_ascending;
    }
    else
    {
        program_config.collection_results.sort_ascending = true;
        sort_ascending = true;
        program_config.collection_results.sort_mask = mask;
        sort_mask = mask;
    }

    var mask_names = ["name", "number", "date", "cost", "type"];
    var indicator = document.getElementById("c-"+mask_names[mask]+"-sorting-arrow");

    for(var i = 0; i < mask_names.length; i++)
    {
        var indicator = document.getElementById("c-"+mask_names[i]+"-sorting-arrow");
        if(i === sort_mask)
        {
            if( sort_ascending === true )
            {
                indicator.src = "resource/arrow_down.png";
            }
            else
            {
                indicator.src = "resource/arrow_up.png";
            }
        }
        else
        {
            indicator.src = "resource/arrow_dual.png";
        }
    }
    var resultlines = document.getElementById("collection-result-line-container").children;
    for(var i = 0; i < resultlines.length; i++)
    {
        if(i % 2 === 1)
        {
             resultlines[i].classList.add("result-grey-background");
        }
        else
        {
             resultlines[i].classList.remove("result-grey-background");
        }
    }
}
