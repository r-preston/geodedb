function redbox(box)
{
    document.getElementById(box).style.boxShadow = "inset 0px 0px 4px red";
}

function remove_collection_image(index)
{
    var target = document.getElementById("c-edit-images-visible").children[index];
    target.parentElement.removeChild(target);
    var path = document.getElementById("c-edit-images").children[index];
    var removed = path.innerText;
    path.parentElement.removeChild(path);
    ui.remove_collection_image(removed);
}

function remove_mineral_image(index)
{
    var target = document.getElementById("m-edit-images-visible").children[index];
    target.parentElement.removeChild(target);
    var path = document.getElementById("m-edit-images").children[index];
    var removed = path.innerText;
    path.parentElement.removeChild(path);
    ui.remove_mineral_image(removed);
}

function add_mineral_image_path(path, abort)
{
    var container = document.getElementById("m-edit-images-visible");

    var newline = document.createElement("div");
    var newpath = document.createElement("div");
    var newcross = document.createElement("img");
    var newtext = document.createTextNode(namepart(path));

    newline.classList.add("image-path-container");
    newpath.classList.add("image-path");
    newcross.classList.add("image-path-remover");
    newcross.src = "resource/x_over.png";

    var hiddenpaths = document.getElementById("m-edit-images");
    var hidepath = document.createElement("div");
    var hidetext = document.createTextNode('');
    hidepath.appendChild(hidetext);
    hidepath.innerText = path;
    hiddenpaths.appendChild(hidepath);

    newcross.onclick = function()
                       {
                           var i = 0;
                           var node = this.parentElement;
                           while(node.previousSibling)
                           {
                               node = node.previousSibling;
                               i++;
                           }
                           ui.remove_mineral_image(i);
                       };

    newpath.appendChild(newtext);
    newline.appendChild(newcross);
    newline.appendChild(newpath);

    if(abort === true) return;

    container.appendChild(newline);
}

function add_collection_image_path(path, abort)
{
    var container = document.getElementById("c-edit-images-visible");

    var newline = document.createElement("div");
    var newpath = document.createElement("div");
    var newcross = document.createElement("img");
    var newtext = document.createTextNode(namepart(path));

    newline.classList.add("image-path-container");
    newpath.classList.add("image-path");
    newcross.classList.add("image-path-remover");
    newcross.src = "resource/x_over.png";

    var hiddenpaths = document.getElementById("c-edit-images");
    var hidepath = document.createElement("div");
    var hidetext = document.createTextNode('');
    hidepath.appendChild(hidetext);
    hidepath.innerText = path;
    hiddenpaths.appendChild(hidepath);

    newcross.onclick = function()
                       {
                           var i = 0;
                           var node = this.parentElement;
                           while(node.previousSibling)
                           {
                               node = node.previousSibling;
                               i++;
                           }
                           ui.remove_collection_image(i);
                       };

    newpath.appendChild(newtext);
    newline.appendChild(newcross);
    newline.appendChild(newpath);

    if(abort === true) return;

    container.appendChild(newline);
}

function get_mineral_data()
{
    document.getElementById("m-edit-pagenumber" ).style.boxShadow = "none";

    var mentry = new MineralEntry();
    var numbuffer, imgbuffer;

    mentry.name        =         document.getElementById("m-edit-name"       ).innerText.replace(/\n/g, '');
    mentry.varieties   = explode(document.getElementById("m-edit-varieties"  ).innerText.replace(/\n/g, ''), ",");
    mentry.structure   =         document.getElementById("m-edit-structure"  ).innerText.replace(/\n/g, '');
    mentry.description =         document.getElementById("m-edit-description").innerText;
    mentry.colours     = explode(document.getElementById("m-edit-colours"    ).innerText.replace(/\n/g, ''), ",");
    mentry.composition =         document.getElementById("m-edit-composition").innerText.replace(/\n/g, '');
    mentry.category    =         document.getElementById("m-edit-category"   ).innerText.replace(/\n/g, '');
    numbuffer          =         document.getElementById("m-edit-pagenumber" ).innerText.replace(/\n/g, '');
    imgbuffer          =         document.getElementById("m-edit-images"     ).children;

    mentry.images = [];
    for(var i = 0; i < imgbuffer.length; i++)
    {
        mentry.images.push(imgbuffer[i].innerText.replace(/\n/g, ''));
    }

    mentry.description = mentry.description.replace(/<div>/g, '\n');
    mentry.description = mentry.description.replace(/<\/div>/g, '');

    if(!is_integer(numbuffer))
    {
        status("Page number should be a positive integer", true);
        redbox("m-edit-pagenumber");
        return false;
    }
    else
    {
        mentry.pagenumber = (parseInt(numbuffer, 10) !== parseInt(numbuffer, 10)) ? -1 : parseInt(numbuffer, 10);
    }

    mentry.name        = mentry.name.replace(/&nbsp;/g, " ");
    mentry.structure   = mentry.structure.replace(/&nbsp;/g, " ");
    mentry.description = mentry.description.replace(/&nbsp;/g, " ");
    mentry.composition = mentry.composition.replace(/&nbsp;/g, " ");
    mentry.category    = mentry.category.replace(/&nbsp;/g, " ");

    if(mentry.description.substr(-1,1) === "\n")
    {
        mentry.description = mentry.description.substr(0, mentry.description.length - 1);
    }
    if(mentry.description.length > 0)
    {
        if(mentry.description[0] === "\n")
        {
            mentry.description = mentry.description.substr(1);
        }
    }

    if(mentry.pagenumber !== mentry.pagenumber)
    {
        mentry.cost = -1;
    }

    return mentry;
}

function get_collection_data()
{
    document.getElementById("c-edit-date-day" ).style.boxShadow = "none";
    document.getElementById("c-edit-date-month" ).style.boxShadow = "none";
    document.getElementById("c-edit-date-year" ).style.boxShadow = "none";
    document.getElementById("c-edit-cost" ).style.boxShadow = "none";
    document.getElementById("c-edit-number" ).style.boxShadow = "none";
    document.getElementById("c-edit-order" ).style.boxShadow = "none";

    var mentry = new CollectionEntry();
    var numbuffer, costbuffer, imgbuffer, dbufday, dbufmnth, dbufyr;

    numbuffer            =         document.getElementById("c-edit-number"       ).innerText.replace(/\n/g, '');
    mentry.order         =         document.getElementById("c-edit-order"        ).innerText.replace(/\n/g, '');
    mentry.name          =         document.getElementById("c-edit-name"         ).innerText.replace(/\n/g, '');
    mentry.type          = explode(document.getElementById("c-edit-type"         ).innerText.replace(/\n/g, ''), ",");
    mentry.description   =         document.getElementById("c-edit-description"  ).innerText;
    mentry.notes         =         document.getElementById("c-edit-notes"        ).innerText;
    mentry.purchaseplace =         document.getElementById("c-edit-purchaseplace").innerText.replace(/\n/g, '');
    costbuffer           =         document.getElementById("c-edit-cost"         ).innerText.replace(/\n/g, '');
    mentry.buyer         =         document.getElementById("c-edit-buyer"        ).innerText.replace(/\n/g, '');
    mentry.colours       = explode(document.getElementById("c-edit-colours"      ).innerText.replace(/\n/g, ''), ",");
    mentry.location      =         document.getElementById("c-edit-location"     ).innerText.replace(/\n/g, '');
    imgbuffer            =         document.getElementById("c-edit-images"       ).children;

    mentry.images = [];
    for(var i = 0; i < imgbuffer.length; i++)
    {
        mentry.images.push(imgbuffer[i].innerText.replace(/\n/g, ''));
    }

    dbufday  = document.getElementById("c-edit-date-day").innerText;
    dbufmnth = document.getElementById("c-edit-date-month").innerText;
    dbufyr   = document.getElementById("c-edit-date-year").innerText;

    var radios = document.getElementsByName("c-edit-texture");
    for(var i = 0; i < radios.length; i++)
    {
        if(radios[i].checked)
        {
            mentry.texture = parseInt(radios[i].value, 10);
            break;
        }
    }

    mentry.description = mentry.description.replace(/<div>/g, '\n');
    mentry.description = mentry.description.replace(/<\/div>/g, '');
    mentry.notes = mentry.notes.replace(/<div>/g, '\n');
    mentry.notes = mentry.notes.replace(/<\/div>/g, '');

    if(mentry.description.substr(-1,1) === "\n")
    {
        mentry.description = mentry.description.substr(0, mentry.description.length - 1);
    }
    if(mentry.description.length > 0)
    {
        if(mentry.description[0] === "\n")
        {
            mentry.description = mentry.description.substr(1);
        }
    }
    if(mentry.notes.substr(-1,1) === "\n")
    {
        mentry.notes = mentry.notes.substr(0, mentry.notes.length - 1);
    }
    if(mentry.notes.length > 0)
    {
        if(mentry.notes[0] === "\n")
        {
            mentry.notes = mentry.notes.substr(1);
        }
    }

    if(!is_integer(numbuffer))
    {
        status("Purchase number should be a positive integer", true);
        redbox("c-edit-number");
        return false;
    }
    else
    {
        mentry.number = parseInt(numbuffer, 10);
    }

    if(!is_integer(costbuffer))
    {
        status("Cost should be a positive integer", true);
        redbox("c-edit-cost");
        return false;
    }
    else
    {
        mentry.cost = parseInt(costbuffer, 10);
    }

    if(!is_antinumber(mentry.order))
    {
        status("Order should not contain numbers", true);
        redbox("c-edit-order");
        return false;
    }

    mentry.order = mentry.order.replace(/&nbsp;/g, " ");
    mentry.name = mentry.name.replace(/&nbsp;/g, " ");
    mentry.description = mentry.description.replace(/&nbsp;/g, " ");
    mentry.purchaseplace = mentry.purchaseplace.replace(/&nbsp;/g, " ");
    mentry.notes = mentry.notes.replace(/&nbsp;/g, " ");
    mentry.buyer = mentry.buyer.replace(/&nbsp;/g, " ");
    mentry.location = mentry.location.replace(/&nbsp;/g, " ");

    if(mentry.cost !== mentry.cost)
    {
        mentry.cost = -1;
    }
    if(mentry.number !== mentry.number)
    {
        mentry.number = -1;
    }

    if(dbufday.length === 1){
        dbufday = '0'+dbufday;
    }
    if(dbufmnth.length === 1){
        dbufmnth = '0'+dbufmnth;
    }
    if((dbufday.length === 0) && (dbufmnth.length+dbufyr.length > 0)){
        status("Please fill in a day", true);
        redbox("c-edit-date-day");
        return false;
    }
    if((dbufmnth.length === 0) && (dbufday.length+dbufyr.length > 0)){
        status("Please fill in a month", true);
        redbox("c-edit-date-month");
        return false;
    }
    if((dbufyr.length === 0) && (dbufmnth.length+dbufday.length > 0)){
        status("Please fill in a year", true);
        redbox("c-edit-date-year");
        return false;
    }
    if(!is_integer(dbufday) || (parseInt(dbufday, 10) > 31) || (parseInt(dbufday, 10) === 0))
    {
        status("Invalid day", true);
        redbox("c-edit-date-day");
        return false;
    }
    if(!is_integer(dbufmnth) || (parseInt(dbufmnth, 10) > 12) || (parseInt(dbufmnth, 10) === 0))
    {
        status("Invalid month", true);
        redbox("c-edit-date-month");
        return false;
    }
    if(!is_integer(dbufyr) || ((dbufyr.length !== 4) && (dbufyr.length !== 0)))
    {
        status("Invalid year", true);
        redbox("c-edit-date-year");
        return false;
    }

    if(dbufday.length + dbufmnth.length + dbufyr.length === 8)
    {
        mentry.date = dbufday + '-' + dbufmnth + "-" + dbufyr;
    }

    status('Data collected');

    return mentry;
}

function fill_mineral_data(mentry)
{
    var boxes = ["name", "description", "structure", "category", "pagenumber", "colours", "images", "varieties"];
    for(var i = 0; i < boxes.length; i++)
    {
        document.getElementById("m-edit-"+boxes[i]).innerText = '';
    }

    var tmp = '';

    for(var i = 0; i < boxes.length - 3; i++)
    {
        if(typeof mentry[boxes[i]] !== "number")
        {
            if(mentry[boxes[i]].indexOf('\n') === -1)
            {
                document.getElementById("m-edit-"+boxes[i]).innerText = mentry[boxes[i]];
            }
            else
            {
                tmp = mentry[boxes[i]];
                tmp = tmp.replace(/\n/g, "</div><div>") + '</div>'; // replace all newlines with </div><div> pairs, and add final </div>
                tmp.replace("</div>", ''); // remove first </div>
                document.getElementById("m-edit-"+boxes[i]).innerHTML = tmp;
            }
        }
        else if(mentry[boxes[i]] !== -1)
        {
            document.getElementById("m-edit-"+boxes[i]).innerText = mentry[boxes[i]];
        }
    }

    document.getElementById("m-edit-composition").innerText = anti_subscript_numbers(mentry.composition);

    document.getElementById("m-edit-varieties").innerText = list_to_string(mentry.varieties);
    document.getElementById("m-edit-colours").innerText = list_to_string(mentry.colours);

    var container = document.getElementById("m-edit-images-visible");
    while (container.firstChild)
    {
        container.removeChild(container.firstChild);
    }
    container = document.getElementById("m-edit-images");
    while (container.firstChild)
    {
        container.removeChild(container.firstChild);
    }
    for(var i = 0; i < mentry.images.length; i++)
    {
        add_mineral_image_path(mentry.images[i]);
    }
}

function fill_collection_data(centry)
{
    // clear any previous data
    var boxes = ["number", "order", "name", "purchaseplace", "cost", "buyer", "location", "description", "notes", "date-day", "date-month", "date-year", "images", "colours", "type"];
    for(var i = 0; i < boxes.length; i++)
    {
        document.getElementById("c-edit-"+boxes[i]).innerText = '';
    }
    var radios = document.getElementsByName("c-edit-texture");
    for(var i = 0; i < radios.length; i++)
    {
        radios[i].checked = false;
    }

    // add data from centry
    var tmp = '';

    for(var i = 0; i < boxes.length - 6; i++)
    {
        if(typeof centry[boxes[i]] !== "number")
        {
            if(centry[boxes[i]].indexOf('\n') === -1)
            {
                document.getElementById("c-edit-"+boxes[i]).innerText = centry[boxes[i]];
            }
            else
            {
                tmp = centry[boxes[i]];
                tmp = tmp.replace(/\n/g, "</div><div>") + '</div>';
                tmp.replace("</div>", '');
                document.getElementById("c-edit-"+boxes[i]).innerText = tmp;
            }
        }
        else if(centry[boxes[i]] !== -1)
        {
            document.getElementById("c-edit-"+boxes[i]).innerText = centry[boxes[i]];
        }
    }

    document.getElementById("c-edit-type").innerText = list_to_string(centry.type);
    document.getElementById("c-edit-colours").innerText = list_to_string(centry.colours);

    var container = document.getElementById("c-edit-images-visible");
    while (container.firstChild)
    {
        container.removeChild(container.firstChild);
    }
    container = document.getElementById("c-edit-images");
    while (container.firstChild)
    {
        container.removeChild(container.firstChild);
    }
    for(var i = 0; i < centry.images.length; i++)
    {
        add_collection_image_path(centry.images[i]);
    }

    if(centry.date !== '')
    {
        if(centry.date.charAt(0) === '0')
        {
            document.getElementById("c-edit-date-day").innerText = centry.date.substr(1,1);
        }
        else
        {
            document.getElementById("c-edit-date-day").innerText = centry.date.substr(0,2);
        }
        if(centry.date.charAt(3) === '0')
        {
            document.getElementById("c-edit-date-month").innerText = centry.date.substr(4,1);
        }
        else
        {
            document.getElementById("c-edit-date-month").innerText = centry.date.substr(3,2);
        }
        document.getElementById("c-edit-date-year").innerText = centry.date.substr(6,4);
    }

    var radios = document.getElementsByName("c-edit-texture");
    for(var i = 0; i < radios.length; i++)
    {
        if(parseInt(radios[i].value, 10) === centry.texture)
        {
            radios[i].checked = true;
        }
        else {
            radios[i].checked = false;
        }
    }
}

function save_collection_item()
{
    var entry = get_collection_data();
    if(entry !== false)
    {
        document.getElementById("c-edit-date-day" ).style.boxShadow = "none";
        document.getElementById("c-edit-date-month" ).style.boxShadow = "none";
        document.getElementById("c-edit-date-year" ).style.boxShadow = "none";
        document.getElementById("c-edit-cost" ).style.boxShadow = "none";
        document.getElementById("c-edit-number" ).style.boxShadow = "none";
        document.getElementById("c-edit-order" ).style.boxShadow = "none";
        cancel_item_edit(false);
    }
    return entry;
}

function save_mineral_item()
{
    var entry = get_mineral_data();
    if(entry !== false)
    {
        document.getElementById("m-edit-pagenumber" ).style.boxShadow = "none";
        cancel_item_edit(false);
    }
    return entry;
}

function new_item(table)
{
    ui.is_new(true);
    program_config.is_new = true;
    change_rightmost_panel(table[0]+"-edit", false, "new_"+table+"_item");
}

function new_collection_item()
{
    fill_collection_data(new CollectionEntry());
}

function new_mineral_item()
{
    fill_mineral_data(new MineralEntry());
}

function edit_collection_item()
{
    fill_collection_data( program_config.active_collection_item );
    change_rightmost_panel("c-edit");
}

function edit_mineral_item()
{
    fill_mineral_data( program_config.active_mineral_item );
    change_rightmost_panel("m-edit");
}

function cancel_item_edit(ask_user)
{
    // Awesomium wets itself when you give it a function with default parameters, so this line exists instead
    ask_user = (typeof ask_user !== 'undefined') ?  ask_user : true;

    if(ask_user)
    {
        ui.confirm("Changes have not been saved - are you sure you want to cancel?", "cancel_item_edit_2", []);
    }
    else
    {
        cancel_item_edit_2();
    }
}

function cancel_item_edit_2()
{
    ui.is_new(false);
    program_config.is_new = false;
    if(program_config.table === "Minerals")
    {
        change_rightmost_panel("m-view", true);
    }
    else
    {
        change_rightmost_panel("c-view", true)
    }
}

function update_mineral_results()
{
    var columnlist = ["name", "varieties", "description", "colours", "structure", "composition", "images", "category", "pagenumber"];
    var results = document.getElementById("mineral-result-line-container").children;
    for(var i = 0; i < results.length; i++)
    {
        if(results[i].getAttribute("rowid") === program_config.active_mineral_item.rowid.toString())
        {
            for(var j = 0; j < results[i].children.length; j++)
            {
                results[i].children[j].firstChild.firstChild.firstChild.innerText = format_data( program_config.active_mineral_item[columnlist[j]], columnlist[j] );
            }
        }
    }
}

function update_collection_results()
{
    var columnlist = ["number", "order", "name", "type", "description", "colours", "date", "purchaseplace", "notes", "cost", "buyer", "location", "images", "texture"];
    var results = document.getElementById("collection-result-line-container").children;
    for(var i = 0; i < results.length; i++)
    {
        if(results[i].getAttribute("rowid") === program_config.active_collection_item.rowid.toString())
        {
            for(var j = 0; j < results[i].children.length; j++)
            {
                results[i].children[j].firstChild.firstChild.firstChild.innerHTML = format_data( program_config.active_collection_item[columnlist[j]], columnlist[j] );
            }
        }
    }
}
