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
		
	    if (systemInfo.productType === "windows")
	    {
	        installer.setValue("TargetDir", "C:\\QSSWeb2Board");
	    }else{
            installer.setValue("TargetDir", "/tmp/QSSWeb2Board");
        }
    }
}

Component.prototype.createOperations = function()
{
    console.log("Component.prototype.createOperations");

    component.createOperations();

    if (installer.isInstaller())
    {
		Component.prototype.install();
    }
}

Component.prototype.install = function()
{
    component.addElevatedOperation("Execute", "sh", "@TargetDir@/###GUI_INSTALLER###")
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

}

changeLicenseLabels = function()
{
    console.log("changeLicenseLabels = function()");

    page = gui.pageWidgetByObjectName("LicenseAgreementPage");
    page.AcceptLicenseLabel.setText("I accept license above");
    page.RejectLicenseLabel.setText("I do not accept license above");
}
