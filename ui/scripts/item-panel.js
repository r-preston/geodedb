function activate_arrow(table, direction)
{
    var arrow = document.getElementById(table+"-image-arrow-"+direction);

    arrow.src = "resource/arrow_" + direction + ".png";
    arrow.style.cursor = "pointer";
}

function deactivate_arrow(table, direction)
{
    var arrow = document.getElementById(table+"-image-arrow-"+direction);

    arrow.src = "resource/arrow_" + direction + "_null.png";
    arrow.style.cursor = "default";
}

function advance_image(identifier)
{
    var image_list = program_config["active_"+identifier+"_item"].images;

    if(program_config[identifier+"_current_image"] < image_list.length - 1)
    {
        var temp = program_config[identifier+"_current_image"] + 1;
        program_config[identifier+"_current_image"]++;

        document.getElementById(identifier + "-image").style.backgroundImage = "url('"+program_config.image_path+image_list[temp] + "')";
        document.getElementById(identifier + "-image-count").innerHTML = (temp + 1) + "/" + image_list.length;

        activate_arrow(identifier, "left");
        if(temp === image_list.length - 1)
        {
            deactivate_arrow(identifier, "right");
        }
        else
        {
            activate_arrow(identifier, "right");
        }
    }
    return temp;
}

function decrement_image(identifier)
{
    var image_list = program_config["active_"+identifier+"_item"].images;

    if(program_config[identifier+"_current_image"] > 0)
    {
        temp = program_config[identifier+"_current_image"] - 1;
        program_config[identifier+"_current_image"]--;

        document.getElementById(identifier + "-image").style.backgroundImage = "url('"+program_config.image_path+image_list[temp] + "')";
        document.getElementById(identifier + "-image-count").innerHTML = (temp + 1) + "/" + image_list.length;

        activate_arrow(identifier, "right");
        if(temp === 0)
        {
            deactivate_arrow(identifier, "left");
        }
        else
        {
            activate_arrow(identifier, "left");
        }
    }
    return temp;
}

function display_mineral_item()
{
    var mineral_current_image = -1;
    mineral_current_image = program_config.mineral_current_image;

    var m_item = program_config.active_mineral_item;
    var displaybox;

    if(m_item.colours.length > 0) { document.getElementById("view-min-colours-label").style.display = "inline"; }
    else { document.getElementById("view-min-colours-label").style.display = "none"; }
    if(m_item.varieties.length > 0) { document.getElementById("view-min-varieties-label").style.display = "inline"; }
    else { document.getElementById("view-min-varieties-label").style.display = "none"; }
    if(m_item.pagenumber != -1) { document.getElementById("view-min-pagenum-label").style.display = "inline"; }
    else { document.getElementById("view-min-pagenum-label").style.display = "none"; }
    if(m_item.category != "") { document.getElementById("view-min-category-label").style.display = "inline"; }
    else { document.getElementById("view-min-category-label").style.display = "none"; }

    // set name
    displaybox = document.getElementById("mineral-item-name");
    displaybox.innerHTML = '';
    displaybox.innerHTML = m_item.name;

    displaybox = document.getElementById("mineral-item-strucformula");
    displaybox.innerHTML = '';
    if(m_item.structure !== '')
    {
        displaybox.innerHTML = m_item.structure;
        displaybox.innerHTML += ' ';
    }
    displaybox.innerHTML += subscript_numbers(m_item.composition);

    displaybox = document.getElementById("mineral-item-description");
    displaybox.innerHTML = '';
    displaybox.innerHTML = replace_newlines(m_item.description);

    displaybox = document.getElementById("mineral-item-varieties");
    displaybox.innerHTML = '';
    displaybox.innerHTML = display_list(m_item.varieties);

    displaybox = document.getElementById("mineral-item-nickelstrunz");
    displaybox.innerHTML = '';
    displaybox.innerHTML = m_item.category;

    displaybox = document.getElementById("mineral-item-pagenum");
    displaybox.innerHTML = '';
    if(m_item.pagenumber !== -1)
    {
        displaybox.innerHTML = m_item.pagenumber;
    }

    displaybox = document.getElementById("mineral-item-colours");
    displaybox.innerHTML = '';
    displaybox.innerHTML = display_list(m_item.colours);

    // show images and image info
    if(m_item.images.length === 0)
    {
        program_config.mineral_current_image = -1;
        document.getElementById("mineral-image").style.backgroundImage = "url('resource/placeholder.png')";
        document.getElementById("mineral-image-count").innerHTML = "0/0";
        deactivate_arrow("mineral", "left");
        deactivate_arrow("mineral", "right");
    }
    else
    {
        if(mineral_current_image === -1)
        {
            mineral_current_image = 0;
            program_config.mineral_current_image = mineral_current_image;
        }

        var imgpath = "url('" + (program_config.image_path) + m_item.images[mineral_current_image] + "')";
        document.getElementById("mineral-image").style.backgroundImage = imgpath;

        document.getElementById("mineral-image-count").innerHTML = (mineral_current_image + 1) + "/" + m_item.images.length;
        deactivate_arrow("mineral", "left");
        deactivate_arrow("mineral", "right");

        if((mineral_current_image + 1) < m_item.images.length)
        {
            activate_arrow("mineral", "right");
        }
        if((mineral_current_image + 1) > 1)
        {
            activate_arrow("mineral", "left");
        }
    }
    ui.sync_item("mineral_current_image", mineral_current_image);
    return 0;
}

function display_collection_item()
{
    collection_current_image = -1;
    collection_current_image = program_config.collection_current_image;

    var c_item = program_config.active_collection_item

    if(c_item.colours.length > 0) { document.getElementById("view-coll-colours-label").style.display = "inline"; }
    else { document.getElementById("view-coll-colours-label").style.display = "none"; }

    var displaybox;

    // set top line information (number, order, name)
    displaybox = document.getElementById("collection-item-name");
    displaybox.innerHTML = '';

    if(c_item.number !== -1)
    {
        displaybox.innerHTML = c_item.number;
    }
    displaybox.innerHTML += c_item.order;

    var add_structure = ( c_item.number !== -1 ) || ( c_item.order !== '' );
    if(add_structure)
    {
        displaybox.innerHTML += '. ';
    }

    displaybox.innerHTML += c_item.name;

    // show mineral types present
    displaybox = document.getElementById("collection-item-type");
    displaybox.innerHTML = '';
    displaybox.innerHTML = display_list(c_item.type);

    // description
    displaybox = document.getElementById("collection-item-description");
    displaybox.innerHTML = '';
    displaybox.innerHTML = replace_newlines(c_item.description);

    // notes
    displaybox = document.getElementById("collection-item-notes");
    displaybox.innerHTML = '';
    displaybox.innerHTML = replace_newlines(c_item.notes);

    // display purchase info (place of purchase, date, buyer, cost)
    displaybox = document.getElementById("collection-item-purchase");
    displaybox.innerHTML = '';

    add_structure = ( ( c_item.date !== "" ) || ( c_item.buyer !== "" ) ) || ( ( c_item.purchaseplace !== "" ) || ( c_item.cost !== -1 ) );

    if(add_structure)
    {
        displaybox.innerHTML = "Bought";
    }
    if(c_item.purchaseplace !== "")
    {
        displaybox.innerHTML += " in ";
        displaybox.innerHTML += c_item.purchaseplace;
    }
    if(c_item.buyer !== "")
    {
        displaybox.innerHTML += " by ";
        displaybox.innerHTML += c_item.buyer;
    }
    if(c_item.date !== "")
    {
        displaybox.innerHTML += " on the ";
        displaybox.innerHTML += full_date(c_item.date);
    }
    if(c_item.cost !== -1)
    {
        displaybox.innerHTML += " for ";
        displaybox.innerHTML += pence_to_pounds(c_item.cost);
    }

    // show list of colours
    displaybox = document.getElementById("collection-item-colours");
    displaybox.innerHTML = '';
    displaybox.innerHTML = display_list(c_item.colours);

    // show current location
    displaybox = document.getElementById("collection-item-location");
    displaybox.innerHTML = '';
    if(c_item.location !== "")
    {
        displaybox.innerHTML = "Lives in the " + c_item.location;
    }

    // show the texture of the stone
    displaybox = document.getElementById("collection-item-texture");
    displaybox.innerHTML = '';
    if(c_item.texture === 1)
    {
        displaybox.innerHTML = "Tumbled stone";
    }
    else if(c_item.texture === 0)
    {
        displaybox.innerHTML = "Rough stone";
    }

    // show images and image info
    if(c_item.images.length === 0)
    {
        program_config.collection_current_image = -1;
        document.getElementById("collection-image").style.backgroundImage = "url('resource/placeholder.png')";
        document.getElementById("collection-image-count").innerHTML = "0/0";
        deactivate_arrow("collection", "left");
        deactivate_arrow("collection", "right");
    }
    else
    {
        if(collection_current_image === -1)
        {
            collection_current_image = 0
            program_config.collection_current_image = 0;
        }
        document.getElementById("collection-image").style.backgroundImage = "url('"+program_config.image_path+ c_item.images[collection_current_image] + "')";
        document.getElementById("collection-image-count").innerHTML = (collection_current_image + 1) + "/" + c_item.images.length;
        deactivate_arrow("collection", "left");
        deactivate_arrow("collection", "right");
        if((collection_current_image + 1) < c_item.images.length)
        {
            activate_arrow("collection", "right");
        }
        if((collection_current_image + 1) > 1)
        {
            activate_arrow("collection", "left");
        }
    }
    ui.sync_item("collection_current_image", collection_current_image);
    return 0;
}

function confirm_delete_item(type)
{
    ui.confirm("Are you sure you want to delete this item?", "delete_"+type+"_item", []);
}

function delete_collection_item()
{
    var a = ui.delete_collection_item();
    if(a)
    {
        status("Item deletion unsuccessful", true);
        return 1;
    }

    var results = document.getElementById("collection-result-line-container").children;
    for(var i = 0; i < results.length; i++)
    {
        if(results[i].getAttribute("rowid") === program_config.active_collection_item.rowid.toString())
        {
            results[i].parentNode.removeChild(results[i]);
        }
    }

    reset_collection_object(program_config.active_collection_item);
    display_collection_item();
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
    status("Item deleted");
}

function delete_mineral_item()
{
    var a = ui.delete_mineral_item();
    if(a)
    {
        status("Item deletion unsuccessful", true);
        return 1;
    }

    var results = document.getElementById("mineral-result-line-container").children;
    for(var i = 0; i < results.length; i++)
    {
        if(results[i].getAttribute("rowid") === program_config.active_mineral_item.rowid.toString())
        {
            results[i].parentNode.removeChild(results[i]);
        }
    }

    reset_mineral_object(program_config.active_mineral_item);
    display_mineral_item();
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
    status("Item deleted");
}
