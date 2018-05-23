function Controller()
{
    if ( installer.isInstaller())
    {
        installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
    }

    installer.setMessageBoxAutomaticAnswer("AuthorizationError", QMessageBox.Abort); // Do not show AuthorizationError default window
}

// Introduction Page

Controller.prototype.IntroductionPageCallback = function()
{
    console.log("Controller.prototype.IntroductionPageCallback");

    var currentPage = gui.currentPageWidget();

    if ( installer.isInstaller())
    {
        currentPage.title = "QSSWeb2Board Installer"
        currentPage.MessageLabel.text = "Welcome to QSSWeb2Board installer.\n\nThis installer will guide you through the software installation needed to use Bitbloq with QSSWeb2Board.";
    }
}

// Target Directory Page

Controller.prototype.TargetDirectoryPageCallback = function()
{
    var currentPage = gui.currentPageWidget();
    currentPage.TargetDirectoryLineEdit.disabled = true;
    //currentPage.TargetDirectoryLineEdit.textChanged.connect(this, Controller.prototype.targetChanged);
    //currentPage.MessageLabel.text = "Select an installation folder:";
    //Controller.prototype.targetChanged(currentPage.TargetDirectoryLineEdit.text);
}

Controller.prototype.targetChanged = function (text)
{
    var currentPage = gui.currentPageWidget();

    if (text != "" && installer.fileExists(text + "/components.xml"))
    {
        currentPage.WarningLabel.text = "\n\tThis folder already contains an installation.\n\n\tPlease uninstall any previous version of Zetup before installing a new one.";
    }
    else
    {
        currentPage.WarningLabel.text = "";
    }
}

// Component Selection Page

Controller.prototype.ComponentSelectionPageCallback = function()
{
    console.log("Controller.prototype.ComponentSelectionPageCallback");
}

// License Agreement Page

Controller.prototype.LicenseAgreementPageCallback = function()
{
    console.log("Controller.prototype.LicenseAgreementPageCallback");

    installer.setValue("RemoveTargetDir", true); // Allow uninstaller remove target directory
}

// Ready for Installation Page

Controller.prototype.ReadyForInstallationPageCallback = function()
{
    console.log("Controller.prototype.ReadyForInstallationPageCallback");

    if (installer.isUpdater())
    {
        console.log("Controller.prototype.ReadyForInstallationPageCallback(): Skipping page");

        gui.clickButton(buttons.CommitButton);
    }
}

Controller.prototype.PerformInstallationPageCallback = function()
{
    console.log("Controller.prototype.PerformInstallationPageCallback");
}

Controller.prototype.FinishedPageCallback = function()
{
    console.log("Controller.prototype.FinishedPageCallback");

    if (installer.isUpdater())
    {
        console.log("Controller.prototype.FinishedPageCallback(): Skipping page");

        gui.clickButton(buttons.FinishButton);
    }
}
