function namepart(filepath)
{
    var n = filepath.lastIndexOf('/');
    return filepath.slice(n + 1);
}

function replace_newlines(input)
{
    output = "<div>";
    output += input;
    output = output.replace(/\n/g, "</div><div>");
    output += "</div>";
    return output;
}

function list_comp(lst_one, lst_two)
{
    if(lst_one.length !== lst_two.length) { return false; }
    for(var i = 0; i < lst_one.length; i++)
    {
        if(lst_one[i] !== lst_two[i])
        {
            return false;
        }
    }
    return true;
}

function make_date_acceptable(search_form)
{
    // meaning of return values:
    //    0 means date is unacceptable
    //    anything else means date is fine

    var day = '';
    var month = '';
    var year = '';

    day   += search_form.elements["date-day-search"].value;
    month += search_form.elements["date-month-search"].value;
    year  += search_form.elements["date-year-search"].value;

    if( (day === '') && (month === '') && (year === '') ) return "";

    if( !is_integer( day )) return 0;
    if( !is_integer( month )) return 0;
    if( !is_integer( year )) return 0;

    if( day === '')
    {
        day = "XX";
    }
    else
    {
        if( parseInt(day, 10) > 31 ) return 0;
        if( parseInt(day, 10) < 1 ) return 0;
    }

    if( month === '')
    {
        month = "XX";
    }
    else
    {
        if( parseInt(month, 10) > 12 ) return 0;
        if( parseInt(month, 10) < 1 ) return 0;
    }

    if( year === '')
    {
        year = "XXXX";
    }
    else
    {
        if( parseInt(year, 10) === 0 ) return 0;
        if( year.length !== 4 ) return 0;
    }

    if(day.length === 1) day = "0" + day;
    if(month.length === 1) month = "0" + month;

    return (day + "-" + month + "-" + year);
}

function subscript_numbers(raw)
{
    var output = "";
    var insertafter = false;
    var digits = "0123456789";
    var precedents = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ)";

    if(raw.length > 0)
    {
        output += raw[0];
    }
    for(var i = 1; i < raw.length; i++)
    {
        if((digits.indexOf(raw[i]) === -1) && (insertafter)) // if n is a letter and a <sub> needs closing
        {
            output += "</sub>";
            insertafter = false;
        }
        if((digits.indexOf(raw[i]) !== -1) && (precedents.indexOf(raw[i - 1]) !== -1) && (!insertafter)) // if n is the first digit of a number
        {
            output += "<sub>";
            insertafter = true;
        }
        i
        output += raw[i];
    }
    if(insertafter)
    {
        output += "</sub>";
    }
    return output;
}

function anti_subscript_numbers(raw)
{
    raw = raw.replace(/<div>/g, "");
    return raw.replace(/<\/div>/g, "");
}

function parseNum(value)
{
    if(value === '') return -1;
    if(!is_integer(value)) return -1;
    return parseInt(value, 10);
}

function list_to_string(l, concat)
{
    concat = (typeof concat !== 'undefined') ?  concat : ", ";
    var res = '';
    for(var i = 0; i < l.length; i++)
    {
        res += l[i] + concat;
    }
    res = res.slice(0, (-1*concat.length));
    return res;
}

function full_date(datestring, abbrev)
{
    // Awesomium wets itself when you give it a function with default parameters, so this line exists instead
    abbrev = (typeof abbrev !== 'undefined') ?  abbrev : false;

    var full_date = '';
    var months = ["January","February","March","April","May","June","July","August","September","October","November","December"];

    var day = datestring.substr(0,2);

    if(day[0] === '0')
    {
        full_date = day[1];
    }
    else
    {
        full_date = day;
    }

    if(day[0] === '1')
    {
        full_date += 'th ';
    }
    else if(day[1] === '1')
    {
        full_date += 'st ';
    }
    else if(day[1] === '2')
    {
        full_date += 'nd ';
    }
    else if(day[1] === '3')
    {
        full_date += 'rd ';
    }
    else
    {
        full_date += 'th ';
    }

    var month = '';
    if(abbrev)
    {
        month = months[parseInt(datestring.substr(3,5), 10) - 1].substr(0,3);
    }
    else
    {
        month = months[parseInt(datestring.substr(3,5), 10) - 1];
    }
    full_date += month + ' ';

    full_date +=  datestring.substr(6,10);

    return full_date;
}

function pence_to_pounds(pence, prefix)
{
    prefix = (typeof prefix !== 'undefined') ?  prefix : "&pound;";
    var money = '';

    if(pence < 100)
    {
        money = pence + "p";
        return money;
    }

    money += (pence/100).toString();

    // works out the log to base 10 of the number, which is equal to the number of figures before the decimal point minus 1
    // this was intended to use Math.log10, but this caused crashes for some reason
    // now using the change of base formula, as Math.log() works fine
    var beforepoint = Math.floor(Math.log(pence/100) / Math.log(10));
    if(beforepoint < 0) beforepoint = 0; // 0 means that the number is < 10

    // crop the string to 2 decimal places
    // reason for the +4:
    //   beforepoint is equal to number of figures before the decimal point minus 1, so add 1 to make up for that
    //   1 is added because of the decimal point
    //   2 is added for the 2 decimal places
    money = money.substr(0, beforepoint + 4);

    if( (pence/100) !== (Math.floor(pence/100)))
    {
        // only add zeroes if the number has a number of pence, i.e. pence/100 is not an integer
        while(money.length < beforepoint + 4)
        {
            money += "0"; // add zeroes on the end because aesthetics
        }
    }

    money = prefix + money; // add pound sign

    return money; // if only...
}

function display_list(var_list)
{
    var res = '';
    if(var_list.length < 1)
    {
        return res;
    }
    if(var_list.length === 1)
    {
        res += var_list[0];
        return res;
    }
    for(var i = 0; i < var_list.length - 2; i++)
    {
        res += var_list[i];
        res += ', ';
    }
    res += var_list[var_list.length - 2];
    res += ' and ';
    res += var_list[var_list.length - 1];

    return res;
}

function explode(s, c)
{
    // splits a string s on c
    // for example, explode("list,of,words",',') returns ["list","of","words"]
    // function ignores any instances of c inside double quotes

	var buffer = ''; // stores current word
	var v = []; // output
	var track = true; // keeps track of whether the loop is in a double quote pair or not

	for(var i = 0; i < s.length; i++) // iterate through s
	{
	    if(s[i] === '\"')
        {
            track = !track; // we have just entered/left a quote pair
        }
	    if(!track)
        {
            buffer += s[i]; // if in quote pair, add n to buffer regardless
            continue; // skip any further comparisons
        }
		if(s[i] !== c)
        {
            buffer += s[i]; // if n is not c, add to buffer
        }
        else if((s[i] === c) && (buffer !== "") && track)
        {
            v.push(buffer); // if n is c, add buffer to output
            buffer = ''; // reset buffer
        }
	}
	if(buffer !==  '') // if there is something in the buffer
    {
        v.push(buffer); // add whatever buffer is to output
    }

    // remove trailing whitespace from array elements
    for(var j = 0; j < v.length; j++)
    {
        v[j] = v[j].replace(/&nbsp;/g, " ");
        while(v[j].charAt(0) === ' ')
        {
            v[j] = v[j].slice(1);
        }
        while(v[j].slice(-1) === ' ')
        {
            v[j] = v[j].slice(0, -1);
        }
    }

	return v;
}

function is_integer(s, allow_void)
{
    // checks if s contains only numbers, i.e. s is an integer
    // by default, '~' is accepted, which is translated as NULL

    // Awesomium wets itself when you give it a function with default parameters, so this line exists instead
    allow_void = (typeof allow_void !== 'undefined') ?  allow_void : true;

    if(s == "~" && allow_void)
    {
        return true;
    }
    var digits = "0123456789";
    for(var i = 0; i < s.toString().length; i++) // iterate through s
    {
        if(digits.indexOf(s.toString()[i]) === -1) // if n is not a number
        {
            return false;
        }
    }
    return true; // s is a number
}

function is_antinumber(s)
{
    // opposite of is_integer - returns false if s contains any numbers

    var digits = "0123456789";
    for(var i = 0; i < s.length; i++) // iterate through the string
    {
        if(digits.indexOf(s[i]) !== -1) // n is a number! D:
        {
            return false;
        }
    }
    return true; // no number here
}

function censor(word)
{
    if(word == null) { return ''; }
    var valid_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    var result = '';

    for(var i = 0; i < word.length; i++)
    {
        if(valid_chars.indexOf(word[i]) !== -1)
        {
            result += word[i];
        }
    }

    return result;
}

function reset_collection_object(obj)
{
    obj.rowid = -1;
    obj.number = -1;
    obj.order = '';
    obj.name = '';
    obj.type = [];
    obj.description = '';
    obj.colours = [];
    obj.date = '';
    obj.purchaseplace = '';
    obj.notes = '';
    obj.cost = -1;
    obj.buyer = '';
    obj.location = '';
    obj.images = [];
    obj.texture = -1;
}

function reset_mineral_object(obj)
{
    obj.rowid = -1;
    obj.name = '';
    obj.varieties = [];
    obj.description = '';
    obj.colours = [];
    obj.structure = '';
    obj.composition = '';
    obj.images = [];
    obj.category = '';
    obj.pagenumber = -1;
}

function push_to_list(config_property, value)
{
    var temp = program_config[config_property];
    temp.push(value);
    program_config[config_property] = temp;
    ui.sync_item(config_property, temp);
    return temp;
}

function modify_list(config_property, index, value)
{
    var temp = program_config[config_property];
    temp[index] = value;
    program_config[config_property] = temp;
    ui.sync_item(config_property, temp);
    return temp;
}

function remove_one_item(config_property, index)
{
    var temp = program_config[config_property];
    temp.splice(index, 1);
    program_config[config_property] = temp;
    ui.sync_item(config_property, temp);
    return temp;
}
