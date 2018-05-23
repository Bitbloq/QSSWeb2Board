Component.prototype.createOperations = function(){
	try{
		// call the base create operations function
		component.createOperations();
		if (installer.value("os") === "win"){
		component.addOperation("Execute", "regedit.exe /S","@TargetDir@\res\QSSWeb2Board.reg");
		}
	} catch (e){
		print(e);
	}
}
