function move_element_down(element)
{
    if (element.nextElementSibling)
    {
        element.parentElement.insertBefore(element, element.nextElementSibling.nextElementSibling);
    }
}

function sort_mineral_items()
{
    var itemlist = document.getElementById("mineral-result-line-container").children;
    var track_ids = [];

    for(var k = 0; k < itemlist.length; k++)
    {
        track_ids.push(parseInt(itemlist[k].getAttribute("rowid"),10));
    }

    while(!list_comp(track_ids, program_config.minerals_results.results))
    {
        for(var j = 0; j < itemlist.length; j++)
        {
            if(track_ids[j] !== program_config.minerals_results.results[j])
            {
                track_ids[j] = track_ids[j + 1] ^ track_ids[j];
                track_ids[j + 1]     = track_ids[j + 1] ^ track_ids[j];
                track_ids[j] = track_ids[j + 1] ^ track_ids[j];
                move_element_down(itemlist[j]);
                if(track_ids[j + 1] === program_config.minerals_results.results[j + 1])
                {
                    break;
                }
            }
        }
    }
}

function sort_collection_items()
{
    var itemlist = document.getElementById("collection-result-line-container").children;
    var track_ids = [];

    for(var k = 0; k < itemlist.length; k++)
    {
        track_ids.push(parseInt(itemlist[k].getAttribute("rowid"),10));
    }

    while(!list_comp(track_ids, program_config.collection_results.results))
    {
        for(var j = 0; j < itemlist.length; j++)
        {
            if(track_ids[j] !== program_config.collection_results.results[j])
            {
                track_ids[j] = track_ids[j + 1] ^ track_ids[j];
                track_ids[j + 1]     = track_ids[j + 1] ^ track_ids[j];
                track_ids[j] = track_ids[j + 1] ^ track_ids[j];
                move_element_down(itemlist[j]);
                if(track_ids[j + 1] === program_config.collection_results.results[j + 1])
                {
                    break;
                }
            }
        }
    }
}
