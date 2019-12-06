using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;
using Xamarin.Forms.Xaml;
using Tizen.Wearable.CircularUI.Forms;
using NStopWatch = System.Diagnostics.Stopwatch;
using Tizen.System;
using Google;

namespace TizenWearableXamlApp1
{
    [XamlCompilation(XamlCompilationOptions.Compile)]
    public partial class MainPage : CirclePage
	{
        // stop watch for measuring elapsed time
        NStopWatch _mainStopWatch;
        String lastCode;
        double stayTime;

        public MainPage ()
		{
			InitializeComponent ();
            _mainStopWatch = new NStopWatch();
            lastCode = "";
            stayTime = 30000.0;
        }

        public void Start()
        {
            //Device.BeginInvokeOnMainThread(() => Power.RequestCpuLock(0));
            //_mainStopWatch.Start();

            Timebar.IsVisible = true;

            // Synchronize the TimeSpan property on every 10 milliseconds.
            // Minimum time that presented in Central Label is 10 milliseconds.

            // Rotate Red and Blue Bar on every 16 milliseconds to fit 60 fps.
            //Device.StartTimer(TimeSpan.FromMilliseconds(20), OnTimeBarAnimate);
            OnComputeGoogleCode();
            Device.StartTimer(TimeSpan.FromMilliseconds(33), OnTimeBarAnimate);
            Device.StartTimer(TimeSpan.FromMilliseconds(1000), OnComputeGoogleCode);
        }
        public void Pause()
        {

            // Power.ReleaseCpuLock() is deperecated but there are no alternative on TizenFX 4.0
            // Use ReleaseLock(PowerLock) if you use version after TizenFX 5.0.
            //Device.BeginInvokeOnMainThread(() => Power.ReleaseCpuLock());

            //_mainStopWatch.Stop();
            //Device.Cancle

        }

        /// <summary>
        /// the method present the Red, Blue Bar and the circular progressbar animation.
        /// </summary>
        /// <returns>the timer will keep recurring if return true.</returns>
        bool OnTimeBarAnimate()
        {
            double timeBarValue = (stayTime / 30000.0) % 1;

            Timebar.Value = timeBarValue;
            stayTime = stayTime - 33;
            
            return true;
        }
        bool Reset()
        {
           
            return true;
        }
        bool OnComputeGoogleCode()
        {
            long duration = 30000;
            string key = "aaaaaaaaaaaaaaaaaaaaaaaa";
            GoogleAuthenticator authenticator = new GoogleAuthenticator(duration, key);
            var mobileKey = authenticator.GetMobilePhoneKey();

            Console.WriteLine("手机端秘钥为：" + mobileKey);

            var code = authenticator.GenerateCode();
            Console.WriteLine("动态验证码为：" + code);

            //Console.WriteLine("刷新倒计时：" + authenticator.EXPIRE_SECONDS);

            //System.Threading.Thread.Sleep(1000);
            //Console.Clear();
            if(stayTime == 0)
            {
                stayTime = authenticator.EXPIRE_SECONDS;
            }
            
            if (lastCode.Equals(code))
            {
                OnTimeBarAnimate();
            }
            else
            {
                stayTime = authenticator.EXPIRE_SECONDS;
                OnTimeBarAnimate();
                lastCode = code;
                StateLabel.Text = "" + code;
            }
            
            return true;
        }
    }
}