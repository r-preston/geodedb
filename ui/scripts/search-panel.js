function undropdown()
{
    var dropmenu = document.getElementById("dropdown-menu");
    if(dropmenu.classList.contains('show-dropdown'))
    {
        dropmenu.classList.remove('show-dropdown');
    }
    var tabcollection = document.getElementById("collection-tab");
    if(tabcollection.classList.contains('dropdown-active'))
    {
        tabcollection.classList.remove('dropdown-active');
    }
    var droparrow = document.getElementById("dropdown-arrow-id");
    if(droparrow.classList.contains('dropdown-arrow-up'))
    {
        droparrow.classList.remove('dropdown-arrow-up');
    }
}

function dropdown()
{
    var x = document.getElementById('dropdown-menu');
    x.classList.toggle("show-dropdown");

    x = document.getElementById('collection-tab');
    x.classList.toggle("dropdown-active");

    x = document.getElementById('dropdown-arrow-id');
    x.classList.toggle("dropdown-arrow-up");
}

function activate_mineral_table()
{
    document.getElementById("mineral-tab").classList.add('tab-active');
    document.getElementById("collection-tab").classList.remove('tab-active');
    program_config.table = "Minerals";
    program_config.active_item_table = "Minerals";

    change_rightmost_panel("m-view", true);

    document.getElementById("collection-search-input-panel").style.display="none";
    document.getElementById("mineral-search-input-panel").style.display="inline";

    document.getElementById("collection-results-panel").style.display="none";
    document.getElementById("mineral-results-panel").style.display="inline";

    if(program_config.minerals_results.search_or)
    {
        document.getElementById("m-radio-or").checked = true;
    }
    else
    {
        document.getElementById("m-radio-and").checked = true;
    }
}

function activate_collection_table()
{
    if(program_config.collection_table === ''){ return 1; }

    program_config.table = program_config.collection_table;
    program_config.active_item_table = program_config.collection_table;

    var tabobject = document.getElementById('collection-tab-text');
    document.getElementById("mineral-tab").classList.remove('tab-active');
    document.getElementById("collection-tab").classList.add('tab-active');
    tabobject.innerHTML = program_config.collection_table;

    document.getElementById("collection-search-input-panel").style.display="inline";
    document.getElementById("mineral-search-input-panel").style.display="none";

    document.getElementById("collection-results-panel").style.display="inline";
    document.getElementById("mineral-results-panel").style.display="none";

    change_rightmost_panel("c-view", true);

    return (' '+program_config.collection_table).slice(1);
}

function choose_new_table()
{
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


    ui.arbitrary_js("activate_collection_table");
    display_collection_item();
}

function clear_mineral_form()
{
    var form = document.getElementById("mineral-search-form");
    var fields = ["main", "name", "variety", "colours", "description", "structure", "composition", "category"];

    for(var i = 0; i < fields.length; i++)
    {
        form.elements[fields[i]+"-search"].value = '';
    }
}

function clear_collection_form()
{
    var form = document.getElementById("collection-search-form");
    var fields = ["main", "name", "purchase-num", "date-day", "date-month", "date-year", "cost", "mineral", "colour", "description", "notes", "purchase", "location", "buyer"];

    for(var i = 0; i < fields.length; i++)
    {
        form.elements[fields[i]+"-search"].value = '';
    }
}

function search_collection_db()
{
    var search_terms = new CollectionEntry();

    var search_form = document.getElementById("collection-search-form");
    var main_search = search_form.elements["main-search"].value;
    search_form.elements["date-day-search"].style.boxShadow = "none";
    search_form.elements["date-month-search"].style.boxShadow = "none";
    search_form.elements["date-year-search"].style.boxShadow = "none";

    // fill search_terms with data
    search_terms.number        = parseNum( search_form.elements["purchase-num-search"].value );
    search_terms.name          =           search_form.elements["name-search"].value;
    search_terms.type          =  explode( search_form.elements["mineral-search"].value, ',' );
    search_terms.description   =           search_form.elements["description-search"].value;
    search_terms.colours       =  explode( search_form.elements["colour-search"].value, ',' );
    search_terms.purchaseplace =           search_form.elements["purchase-search"].value;
    search_terms.notes         =           search_form.elements["notes-search"].value;
    search_terms.cost          = parseNum( search_form.elements["cost-search"].value );
    search_terms.buyer         =           search_form.elements["buyer-search"].value;
    search_terms.location      =           search_form.elements["location-search"].value;

    var modified_date = make_date_acceptable(search_form);
    if( modified_date === 0 )
    {
        status("Malformed date - should be in the form dd-mm-yyyy", true);
        search_form.elements["date-day-search"].style.boxShadow = "inset 0px 0px 4px red";
        search_form.elements["date-month-search"].style.boxShadow = "inset 0px 0px 4px red";
        search_form.elements["date-year-search"].style.boxShadow = "inset 0px 0px 4px red";
        return false;
    }
    search_terms.date = modified_date;

    return [search_terms, main_search];
}

function search_mineral_db()
{
    var search_terms = new MineralEntry();

    var search_form = document.getElementById("mineral-search-form");
    var main_search = search_form.elements["main-search"].value;

    // fill search_terms with data
    search_terms.name        =         search_form.elements["name-search"].value;
    search_terms.varieties   = explode(search_form.elements["variety-search"].value, ',');
    search_terms.description =         search_form.elements["description-search"].value;
    search_terms.colours     = explode(search_form.elements["colours-search"].value, ',');
    search_terms.structure   =         search_form.elements["structure-search"].value;
    search_terms.category    =         search_form.elements["category-search"].value;
    search_terms.composition =         search_form.elements["composition-search"].value;

    return [search_terms, main_search];
}

function create_new_table(name)
{
    choose_new_table(program_config.table);

    var existing_tables = [];
    var dropdown = document.getElementById("dropdown-menu").children;
    for(var i = 0; i < dropdown.length; i++)
    {
        if( dropdown[i].classList.contains("dropdown-table-button") )
        {
            existing_tables.push(dropdown[i].id);
        }
    }
    for(var j = 0; j < program_config.collection_table_list.length; j++)
    {
        if( existing_tables.indexOf(program_config.collection_table_list[j]) === -1 )
        {
            add_new_table( program_config.collection_table_list[j] );
        }
    }
    remove_collection_results();
    display_collection_item();

    status("Table '"+name+"' created");
}

function confirm_delete_table(tablename)
{
    if(!(event == null)) // if this originated from a click event
    {
        event.stopPropagation();
        undropdown();
    }
    ui.confirm("Are you sure you want to delete the table '"+tablename+"'?", "continue_delete_table", [tablename]);
}

function continue_delete_table(tablename)
{
    ui.continue_delete_table(tablename);
}

function remove_table(tablename, prev_table)
{
    status("Starting delete");
    var tables = document.getElementById("dropdown-menu").children;

    for(var i = 0; i < tables.length; i++)
    {
        if((tables[i].id === "dropdown-divider") || (tables[i].id === '') )
        {
            continue;
        }
        var pos = program_config.collection_table_list.indexOf(tables[i].id);
        if( pos === -1 )
        {
            tables[i].parentNode.removeChild(tables[i]);
        }
    }

    if(prev_table === "Minerals")
    {
        document.getElementById("collection-tab-text").innerHTML = program_config.collection_table;
    }
    else if(program_config.table === "Minerals")
    {
        activate_mineral_table();
    }
    else
    {
        ui.arbitrary_js("choose_new_table");
        if(prev_table !== program_config.table)
        {
            remove_collection_results();
            ui.add_collection_items();
        }
    }

    if(program_config.collection_table_list.length === 0)
    {
        document.getElementById("collection-tab-text").innerHTML = '[ none ]';
    }

    status("Table '"+tablename+"' deleted");
    return "done";
}
