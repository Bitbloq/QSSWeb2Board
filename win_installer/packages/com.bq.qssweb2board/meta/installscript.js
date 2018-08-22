function Component()
{
    console.log("Component()");

    installer.installationFinished.connect(this, Component.prototype.installationFinishedPageIsShown);
    installer.finishButtonClicked.connect(this, Component.prototype.installationFinished);
    gui.pageWidgetByObjectName("LicenseAgreementPage").entered.connect(changeLicenseLabels);

    if (installer.isInstaller())
    {
        console.log("Component(): isInstaller()");

		installer.setAutomatedPageSwitchEnabled(false);
		//gui.pageWidgetByObjectName("TargetDirectoryPage").entered.connect(Component.prototype.targetDirectoryPageIsShown);

	    if (systemInfo.productType === "windows")
	    {
	        installer.setValue("TargetDir", "C:\\QSSWeb2Board");
	    }
    }
}

Component.prototype.createOperations = function()
{
    console.log("Component.prototype.createOperations");

    component.createOperations();

    if (installer.isInstaller())
    {
		Component.prototype.createShortcuts();
		Component.prototype.registerApp();
    }
}

Component.prototype.registerApp = function(){

    console.log("Component.prototype.registerApp");
    
    if (systemInfo.productType === "windows"){
        try{
            component.addElevatedOperation("GlobalConfig",
            "HKEY_CLASSES_ROOT\\QSSWeb2Board", 
            "Default", 
            "URL:Bitbloq Protocol");

            component.addElevatedOperation("GlobalConfig",
            "HKEY_CLASSES_ROOT\\QSSWeb2Board", 
            "URL Protocol", 
            "");

            component.addElevatedOperation("GlobalConfig",
            "HKEY_CLASSES_ROOT\\QSSWeb2Board\\shell", 
            "Default", 
            "open");

            component.addElevatedOperation("GlobalConfig",
            "HKEY_CLASSES_ROOT\\QSSWeb2Board\\shell\\open\\command", 
            "Default", 
            "C:\\QSSWeb2Board\\QSSWeb2Board.exe");

        } catch (e){
            print(e);
        }
    }
}

Component.prototype.createShortcuts = function()
{
    console.log("Component.prototype.createShortcuts");

    if (systemInfo.productType === "windows")
    {
        if (component.userInterface("ShortcutCheckBoxForm").shortcutCheckBox.checked)
        {
            component.addElevatedOperation("CreateShortcut", "@TargetDir@/QSSWeb2Board.exe", "@DesktopDir@/QSSWeb2Board.lnk", "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/res/qssweb2board.ico");
        }

        component.addElevatedOperation("CreateShortcut", "@TargetDir@/QSSWeb2Board.exe", "@StartMenuDir@/QSSWeb2Board.lnk", "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/res/qssweb2board.ico");
        component.addElevatedOperation("CreateShortcut", "@TargetDir@/QSSWeb2Board.exe", "@TargetDir@/QSSWeb2Board.lnk", "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/res/qssweb2board.ico");

    }
}

Component.prototype.targetDirectoryPageIsShown = function()
{
    if (installer.isInstaller())
    {
        installer.addWizardPageItem(component, "ShortcutCheckBoxForm", QInstaller.TargetDirectory);
    }
}

Component.prototype.installationFinishedPageIsShown = function()
{
    console.log("Component.prototype.installationFinishedPageIsShown");

    if ((installer.isInstaller() && installer.status == QInstaller.Success) || installer.isUpdater())
    {
        installer.addWizardPageItem(component, "RunAppCheckBoxForm", QInstaller.InstallationFinished);
    }
}

Component.prototype.installationFinished = function()
{
    console.log("Component.prototype.installationFinished");

    //var isRunAppCheckBoxChecked = component.userInterface("RunAppCheckBoxForm").runAppCheckBox.checked;
    var isInstallDriversChecked = component.userInterface("RunAppCheckBoxForm").installDriversCheckBox.checked;

    installer.executeDetached("@TargetDir@/drivers/install_drivers_windows_64bits.bat");
    
    if (((installer.isInstaller() && installer.status == QInstaller.Success) || installer.isUpdater()) && isInstallDriversChecked)
    {
        installer.executeDetached("@TargetDir@/drivers/install_drivers_windows_64bits.bat");

        console.log("Component.prototype.installationFinished(): installing drivers");

        if (systemInfo.productType === "windows"  && systemInfo.currentCpuArchitecture === "i386")
        {
            installer.executeDetached("@TargetDir@/drivers/install_drivers_windows_32bits.bat");
        }
        else if(systemInfo.productType === "windows"  && systemInfo.currentCpuArchitecture === "x86_64" )
        {
            installer.executeDetached("@TargetDir@/drivers/install_drivers_windows_64bits.bat");
        }
    }
/*
    if (((installer.isInstaller() && installer.status == QInstaller.Success) || installer.isUpdater()) && isRunAppCheckBoxChecked)
    {
        console.log("Component.prototype.installationFinished(): executing app");

        if (systemInfo.productType === "windows")
        {
            installer.executeDetached("@TargetDir@/QSSWeb2Board.exe");
        }
    }
    */
}

changeLicenseLabels = function()
{
    console.log("changeLicenseLabels = function()");

    page = gui.pageWidgetByObjectName("LicenseAgreementPage");
    page.AcceptLicenseLabel.setText("I accept all licenses above");
    page.RejectLicenseLabel.setText("I do not accept all licenses above");
}
