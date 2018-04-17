function add_new_table(label)
{
    // adds a new item to the collection table dropdown menu
    var newItem = document.createElement("DIV");
    var textNode = document.createTextNode(label);
    newItem.id = label; // this is the name of the table
    newItem.classList.add("dropdown-table-button")
    newItem.onclick = function() { ui.choose_new_table(this.id); }

    newItem.appendChild(textNode);

    var rm_button = document.createElement("A");
    rm_button.classList.add("remove-button");
    rm_button.onclick = function() { confirm_delete_table(this.parentElement.id); }

    newItem.appendChild(rm_button);

    var dividerElem = document.getElementById("dropdown-divider");
    document.getElementById("dropdown-menu").insertBefore(newItem, dividerElem);
}

function apply_config()
{
    var collection_table = program_config.collection_table;

    if((program_config.collection_table == '') && (program_config.collection_table_list.length > 0))
    {
        collection_table = program_config.collection_table_list[0]
        program_config.collection_table = collection_table;
    }
    if(collection_table !== '')
    {
        document.getElementById("collection-tab-text").innerHTML = program_config.collection_table;
    }
    else
    {
        document.getElementById("collection-tab-text").innerHTML = '[ none ]';
    }

    for(var i = 0; i < program_config.collection_table_list.length; i++)
    {
        add_new_table(program_config.collection_table_list[i]);
    }

    // activate result columns
    var col_col_widths = program_config.collection_column_widths.slice(0);
    var min_col_widths = program_config.mineral_column_widths.slice(0);
    for(var i = 0; i < program_config.visible_collection_columns.length; i++)
    {
        activate_collection_column(program_config.visible_collection_columns[i]);
    }
    for(var i = 0; i < program_config.visible_mineral_columns.length; i++)
    {
        activate_mineral_column(program_config.visible_mineral_columns[i]);
    }

    // set result column widths
    var barwidth = document.getElementById("mineral-results-topbar").offsetWidth;
    for(var i = 0; i < program_config.visible_collection_columns.length; i++)
    {
        set_collection_column_width(program_config.visible_collection_columns[i], col_col_widths[i], barwidth, visible_collection_columns);
    }
    for(var i = 0; i < program_config.visible_mineral_columns.length; i++)
    {
        set_mineral_column_width(program_config.visible_mineral_columns[i], min_col_widths[i], barwidth, visible_mineral_columns);
    }

    if(program_config.table === 'Minerals')
    {
        activate_mineral_table();
    }
    else
    {
        choose_new_table(program_config.table);
        activate_collection_table();
    }

    if(program_config.active_item_table === 'Minerals')
    {
        change_rightmost_panel("m-view", true);
    }
    else
    {
        change_rightmost_panel("c-view", true);
    }
    var label_list = ["name", "number", "date", "cost", "type"];

    for(var i = 0; i < label_list.length; i++)
    {
        if(label_list[i] === label_list[program_config.collection_results.sort_mask])
        {
            var label = document.getElementById("clab-" + label_list[i]);

            for(var j = 0; j < label.childNodes.length; j++)
            {
                if(label.childNodes[j].nodeType == 1)
                {
                    if(label.childNodes[j].classList.contains("sorting-tab-arrow"))
                    {
                        break;
                    }
                }
            }
            for(var k = 0; label.childNodes[j].childNodes[k].tagName !== "IMG"; k++);

            if(program_config.collection_results.sort_ascending)
            {
                label.childNodes[j].childNodes[k].src = "resource/arrow_down.png";
            } else {
                label.childNodes[j].childNodes[k].src = "resource/arrow_up.png";
            }
        }
        else
        {
            var label = document.getElementById("clab-" + label_list[i]);


            for(var j = 0; j < label.childNodes.length; j++)
            {
                if(label.childNodes[j].nodeType == 1)
                {
                    if(label.childNodes[j].classList.contains("sorting-tab-arrow"))
                    {
                        break;
                    }
                }
            }
            for(var k = 0; label.childNodes[j].childNodes[k].tagName !== "IMG"; k++);

            label.childNodes[j].childNodes[k].src = "resource/arrow_dual.png";
        }
    }

    if(program_config.collection_results.search_or)
    {
        document.getElementById("c-radio-or").checked = true;
    }
    else
    {
        document.getElementById("c-radio-and").checked = true;
    }

    var radiolist = document.getElementsByName("datecomp");
    for(var i = 0; i < radiolist.length; i++)
    {
        if(parseInt(radiolist[i].value, 10) === program_config.collection_results.datecomp)
        {
            radiolist[i].checked = true;
        }
    }

    var radiolist = document.getElementsByName("numcomp");
    for(var i = 0; i < radiolist.length; i++)
    {
        if(parseInt(radiolist[i].value, 10) === program_config.collection_results.numcomp)
        {
            radiolist[i].checked = true;
        }
    }

    var radiolist = document.getElementsByName("costcomp");
    for(var i = 0; i < radiolist.length; i++)
    {
        if(parseInt(radiolist[i].value, 10) === program_config.collection_results.costcomp)
        {
            radiolist[i].checked = true;
        }
    }

    var mask_names = ["name", "number", "date", "cost", "type"];

    for(var i = 0; i < mask_names.length; i++)
    {
        var indicator = document.getElementById("c-"+mask_names[i]+"-sorting-arrow");
        if(i === program_config.collection_results.sort_mask)
        {
            if( program_config.collection_results.sort_ascending === true )
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

    if( program_config.minerals_results.sort_ascending === true )
    {
        document.getElementById("m-name-sorting-arrow").src = "resource/arrow_down.png";
    }
    else
    {
        document.getElementById("m-name-sorting-arrow").src = "resource/arrow_up.png";
    }
    return 0;
}

function poke_images()
{
    // the reason for these calls is beyond my ability to fathom
    // if these functions are not called, the image for the image removal button will not load
    // if they are called the image is found fine, even if NOTHING IS ADDED
    // this is just ridiculous and I don't understand it
    add_mineral_image_path("", true);
    add_collection_image_path("", true);

    var arrow = document.getElementById("collection-image-arrow-left");
    arrow.src = "resource/arrow_left_null.png";
    arrow.src = "resource/arrow_left.png";
    arrow = document.getElementById("collection-image-arrow-right");
    arrow.src = "resource/arrow_right_null.png";
    arrow.src = "resource/arrow_right.png";
    arrow = document.getElementById("mineral-image-arrow-left");
    arrow.src = "resource/arrow_left_null.png";
    arrow.src = "resource/arrow_left.png";
    arrow = document.getElementById("mineral-image-arrow-right");
    arrow.src = "resource/arrow_right_null.png";
    arrow.src = "resource/arrow_right.png";

}

function validate_column_widths()
{
    var barwidth = document.getElementById("mineral-results-topbar").offsetWidth;
    for(var i = 0; i < program_config.visible_collection_columns.length; i++)
    {
        var column = document.getElementsByClassName("ccol-" + program_config.visible_collection_columns[i]);
        if(column.length > 1)
        {
            if(column[0].offsetWidth !== column[1].offsetWidth)
            {
                var x = column[0].offsetWidth.toString() + 'px';
                set_collection_column_width(program_config.visible_collection_columns[i], x, barwidth, visible_collection_columns, false);
            }
        }
    }
    for(var i = 0; i < program_config.visible_mineral_columns.length; i++)
    {
        var column = document.getElementsByClassName("mcol-" + program_config.visible_mineral_columns[i]);
        if(column.length > 1)
        {
            if(column[0].offsetWidth !== column[1].offsetWidth)
            {
                var x = column[0].offsetWidth.toString() + 'px';
                set_mineral_column_width(program_config.visible_mineral_columns[i], x, barwidth, visible_mineral_columns, false);
            }
        }
    }
}

function startup_probe()
{
    return [1, program_config.table];
}

function init()
{
    status("Loading program configuration...");
    var a;

    attach_drag_handlers(); // add column resize event listeners
    add_context_listeners(); // add listeners for column context menu

    // make text in editing boxes editable
    var linelist = document.getElementsByClassName('edit-input');
    for(var i = 0; i < linelist.length; i++)
    {
        linelist[i].contentEditable = 'true';
    }

    poke_images();

    apply_config();

    display_mineral_item();
    display_collection_item();

    if(program_config.table === "Minerals")
    {
        a = ui.add_mineral_items();
        a = ui.add_collection_items();
    }
    else
    {
        a = ui.add_collection_items();
        a = ui.add_mineral_items();
    }

    status("Ready");
    return 0;
}
