const output = document.getElementById("output"); 
const button = document.getElementById("runbtn");
const input = document.getElementById("codeinput");

const js_wrap_com_run = Module.cwrap("comp_run" , "string" , ["string"]);

Module['onRuntimeInitialized'] = function() {
	button.innerText = "Run";
	button.disabled = false;
	
	button.onclick = function(){
	
	const source_code = input.value;

	//console.log(source_code);
	const result = js_wrap_com_run(source_code);
	//console.log(result);
	output.value = result;
	}
};



