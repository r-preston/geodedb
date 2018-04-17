function resizer(resizerElement)
{
    // add event listeners to resize regions

    resizerElement.mousemove = function(e)
    {
        var colTarget = this.parentElement.classList[1];
        resizeX(colTarget, e.pageX + 3); // resize box with 3px offset to keep cursor in the middle of the resize region
    }

    resizerElement.onmousedown = function(e)
    {
        document.documentElement.addEventListener('mousemove', resizerElement.dragging, false); // add listener for mouse dragging when the drag region has been clicked
        document.documentElement.addEventListener('mouseup', resizerElement.enddrag, false); // add listener for mouseup event
        if(e.preventDefault) e.preventDefault(); // stop bubbling of event to disable text selection while dragging
    }

    resizerElement.dragging = function(e)
    {
        if (e.which != 1)
        {
            resizerElement.enddrag(e); // end of dragging process
            return;
        }
        resizerElement.mousemove(e); // whlie dragging, resize box to new cursor position
    }

    resizerElement.enddrag = function(e)
    {
        var sizelist = [];
        var percentwidth = 0;
        var tableid = "mineral";
        for(var i = 0; i < this.parentElement.classList.length; i++)
        {
            if(this.parentElement.classList[i].substr(1,4) === "col-")
            {
                if(this.parentElement.classList[i].substr(0,1) === "c")
                {
                    tableid = "collection";
                }
            }
        }

        for(var o = 0; o < program_config["visible_"+tableid+"_columns"].length; o++)
        {
            sizelist.push("");
        }
        var topbar = document.getElementById(tableid+"-results-topbar");
        for( var j = 0; j < topbar.children.length; j++ )
        {
            var colindex = program_config["visible_"+tableid+"_columns"].indexOf(topbar.children[j].id.substr(5));
            if(colindex !== -1)
            {
                percentwidth = ((topbar.children[j].offsetWidth*100)/topbar.offsetWidth).toString()+"%";
                sizelist[colindex] = percentwidth;
            }
        }
        ui.sync_item(tableid+"_column_widths", sizelist);
        document.documentElement.removeEventListener('mouseup', resizerElement.enddrag, false); // ditto for mouseup
        document.documentElement.removeEventListener('mousemove', resizerElement.dragging, false); // when mouseup, stop listening for mouse drag
    }
}

function attach_drag_handlers()
{
    var resizeList = document.getElementsByClassName('resize-region');
    for(var i = 0; i < resizeList.length; i++)
    {
        resizer(resizeList[i]);
    }
    return 0;
}

function resizeX(columnClass, x)
{
    var columns = document.getElementsByClassName(columnClass);
    var barwidth = 0;
    var index = 0;
    if(columnClass.substr(0,1) == "m")
    {
        barwidth = document.getElementById("mineral-results-topbar").offsetWidth;
    }
    else
    {
        barwidth = document.getElementById("collection-results-topbar").offsetWidth;
    }

    var rElem, rect;
    for(var i = 0; i < columns.length; i++)
    {
        rElem = columns[i];
        rect = rElem.getBoundingClientRect(); // get position info for label
        if((x - rect.left) >= 21)
        {
            rElem.style.width = (x - rect.left)*(100/barwidth) + '%'; // set new width to cursor position minus box position
        }
    }
    if(columnClass.substr(0,1) == "m")
    {
        index = program_config.visible_mineral_columns.indexOf(columnClass.substr(5));
        modify_list(program_config.mineral_column_widths, index, (x - rect.left)*(100/barwidth) + '%')
    }
    else
    {
        index = program_config.visible_collection_columns.indexOf(columnClass.substr(5));
        modify_list(program_config.collection_column_widths, index, (x - rect.left)*(100/barwidth) + '%')
    }
}
