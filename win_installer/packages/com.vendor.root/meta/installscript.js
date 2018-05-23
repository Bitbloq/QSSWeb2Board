function Component

Component.prototype.createOperations = function(){
	try{
		// call the base create operations function
		component.createOperations();
		component.addElevatedOperation("GlobalConfig", "company", "Application", "mykey", "myvalue");
	} catch (e){
		print(e);
	}
}
