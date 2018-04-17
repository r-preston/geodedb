function status(text, bad)
{
    // Awesomium wets itself when you give it a function with default parameters, so this line exists instead
    bad = (typeof bad !== 'undefined') ?  bad : false;

    var statbar = document.getElementById('status-wrapper');
    if(bad)
    {
        statbar.style.color = '#aa0000';
        statbar.innerHTML = 'ERROR: ' + text;
    }
    else
    {
        statbar.style.color = 'black';
        statbar.innerHTML = text;
    }
}

function show_error_box(errors, filename)
{
    document.getElementById("add-file-errors-backdrop").style.display = "inline";
    document.getElementById("add-file-error-number").innerHTML = errors.length;
    document.getElementById("add-file-error-name").innerHTML = filename;
    var container = document.getElementById("add-file-error-main");
    var tnode, tdiv;
    for(var i = 0; i < errors.length; i++)
    {
        tnode = document.createTextNode(errors[i]);
        tdiv = document.createElement("div");

        tdiv.appendChild(tnode);
        container.appendChild(tdiv);
    }
}

function show_continue_box(filename)
{
    document.getElementById("add-file-good-backdrop").style.display = "inline";
    document.getElementById("add-file-good-name").innerHTML = filename;
}

function get_add_radio_value()
{
    var radios = document.getElementsByName("fadd-radio");
    for(var i = 0; i < radios.length; i++)
    {
        if(radios[i].checked)
        {
            return radios[i].value;
        }
    }
    return 0;
}

function close_addfile_popups()
{
    document.getElementById("add-file-errors-backdrop").style.display = "none";
    document.getElementById("add-file-good-backdrop").style.display = "none";
    var container = document.getElementById("add-file-error-main");
    while (container.firstChild)
    {
        container.removeChild(container.firstChild);
    }
}

function help_popup()
{
    var help_panel = document.getElementById("id-help-panel");
    help_panel.classList.toggle('show-help-panel');
}

function change_rightmost_panel(panel, force, callback)
{
    // Awesomium wets itself when you give it a function with default parameters, so this line exists instead
    force = (typeof force !== 'undefined') ?  force : false
    callback = (typeof callback !== 'undefined') ?  callback : "";
    if(!force)
    {
        if(document.getElementById("edit-mineral-item-panel").style.display === "inline")
        {
            ui.confirm("Are you sure you want to abandon this edit? All changes will be discarded", "change_rightmost_panel_2", [panel, callback]);
            return;
        }
        else if(document.getElementById("edit-collection-item-panel").style.display === "inline")
        {
            ui.confirm("Are you sure you want to abandon this edit? All changes will be discarded", "change_rightmost_panel_2", [panel, callback]);
            return;
        }
    }
    change_rightmost_panel_2(panel, callback);
}

function change_rightmost_panel_2(panel, callback)
{
    callback = (typeof callback !== 'undefined') ?  callback : "";
    document.getElementById("edit-collection-item-panel").style.display = "none";
    document.getElementById("edit-mineral-item-panel").style.display = "none";
    document.getElementById("view-collection-item-panel").style.display = "none";
    document.getElementById("view-mineral-item-panel").style.display = "none";
    switch(panel)
    {
        case "c-view":
            document.getElementById("view-collection-item-panel").style.display = "inline";
            break;
        case "m-view":
            document.getElementById("view-mineral-item-panel").style.display = "inline";
            break;
        case "c-edit":
            document.getElementById("edit-collection-item-panel").style.display = "inline";
            break;
        case "m-edit":
            document.getElementById("edit-mineral-item-panel").style.display = "inline";
            break;
    }
    if(callback != "")
    {
        window[callback]();
    }

    return true;
}

window.onclick = function(event)
{
    // close help window if user clicks outside of it
    if(!(event.target.id === "id-help-panel") && !(event.target.classList.contains("help-button")))
    {
        document.getElementById('id-help-panel').classList.remove('show-help-panel');
    }
    // close the dropdown menu if the user clicks outside of it
    if(!(event.target.id === 'dropdown-arrow-id'))
    {
        undropdown();
    }

    var someparent = event.target;

    // close the right click menu UNLESS clicking within the menu itself
    while(someparent.parentNode)
    {
        // if the parent is html and loop is still running, the click event is outside the context menu
        if(someparent.tagName === 'HTML')
        {
            var checkbox = document.getElementById("rclick-context-mineral");
            if(checkbox.classList.contains("show-checkbox"))
            {
                checkbox.classList.remove("show-checkbox");
            }
            var checkbox = document.getElementById("rclick-context-collection");
            if(checkbox.classList.contains("show-checkbox"))
            {
                checkbox.classList.remove("show-checkbox");
            }
        }

        // if within the context menu, break the loop (keep menu open)
        if(someparent.id === "rclick-context-mineral")
        {
            break;
        }
        if(someparent.id === "rclick-context-collection")
        {
            break;
        }
        someparent = someparent.parentNode;
    }
}
