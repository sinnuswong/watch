using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;

namespace TizenWearableXamlApp1
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class App : Application
    {
        public App()
        {
            InitializeComponent();

            MainPage = new TizenWearableXamlApp1.MainPage();
        }

        protected override void OnStart()
        {
            // Handle when your app starts
            
        }

        protected override void OnSleep()
        {
            // Handle when your app sleeps
            ((TizenWearableXamlApp1.MainPage)MainPage).Pause(); 
        }

        protected override void OnResume()
        {
            // Handle when your app resumes
            ((TizenWearableXamlApp1.MainPage)MainPage).Start();
        }
    }
}

