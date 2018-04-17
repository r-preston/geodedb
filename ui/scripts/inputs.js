function show_confirm_box(message, cpp_callback, js_callback, js_args, cpp_arg)
{
    document.getElementById("confirm-box-backdrop").style.display = "inline";
    document.getElementById("confirm-box-text").innerHTML = message;
    if(typeof cpp_arg !== 'undefined')
    {
        document.getElementById("confirm-box-go").onclick = function() { ui[cpp_callback](cpp_arg, true, js_callback, js_args) };
        document.getElementById("confirm-box-cancel").onclick = function() { ui[cpp_callback](cpp_arg, false) };
    }
    else
    {
        document.getElementById("confirm-box-go").onclick = function() { ui[cpp_callback](true, js_callback, js_args) };
        document.getElementById("confirm-box-cancel").onclick = function() { ui[cpp_callback](false) };
    }
}

function show_prompt_box(message, callback)
{
    document.getElementById("prompt-box-text").style.boxShadow = "none";
    document.getElementById("prompt-box-backdrop").style.display = "inline";
    document.getElementById("prompt-box-text").innerHTML = message;
    document.getElementById('prompt-box-input').innerHTML = '';
    document.getElementById('prompt-box-input').contentEditable = true;
    document.getElementById('prompt-box-input').focus();
    document.getElementById("prompt-box-go").onclick = function() { ui[callback](document.getElementById('prompt-box-input').innerHTML) };
    document.getElementById("prompt-box-cancel").onclick = function() { ui[callback](false) };
}

function bad_table_name(message)
{
    document.getElementById("prompt-box-text").innerHTML = message;
    redbox('prompt-box-input');
}

function hide_message_boxes()
{
    document.getElementById("prompt-box-backdrop").style.display = "none";
    document.getElementById("confirm-box-backdrop").style.display = "none";
}
