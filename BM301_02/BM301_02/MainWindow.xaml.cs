using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Timers;

namespace BM301_02
{
	

	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		
		private Timer blinkTimer = new Timer(192);
		private delegate void Blink();
		
		
		public MainWindow()
		{
			this.InitializeComponent();
				            
            blinkTimer.Elapsed += new ElapsedEventHandler(BlinkTimer_Elapsed);
            blinkTimer.Enabled = true;

			// Insert code required on object creation below this point.
		}
		
		
		private void BlinkTimer_Elapsed(object sender, ElapsedEventArgs e)
        {
            Dispatcher.Invoke(new Blink(OnBlink));            
        }

        //Мигаем параметрами, если надо
        private void OnBlink()
        {                        
            blinkTimer.Start();
			
			if (Vpr.Foreground == Brushes.White) 
			{
				Vpr.Foreground = Brushes.Black;
				Vpr.Background = Brushes.Lime;
				V_pr_detail.Foreground = Brushes.Black;
				V_pr_short.Foreground = Brushes.Black;
				V_pr_detail.Background = Brushes.Lime;
				V_pr_short.Background = Brushes.Lime;
				
			}
			else 
			{
				Vpr.Foreground = Brushes.White;
				Vpr.Background = Brushes.Black;
				V_pr_detail.Foreground = Brushes.White;
				V_pr_short.Foreground = Brushes.White;
				V_pr_detail.Background = Brushes.Black;
				V_pr_short.Background = Brushes.Black;
			}			
        }
		
		
		void M1_Click(object sender, EventArgs e)
		{
			//Вкл/откл АП
			
			if (Field_1_1.Visibility == Visibility.Visible)
			{
				Field_1_1.Visibility = Visibility.Hidden;
				Field_1_2.Visibility = Visibility.Hidden;
				Field_1_3.Visibility = Visibility.Hidden;
				Field_1_4.Visibility = Visibility.Hidden;
				ap.Foreground = Brushes.White;
				ap.Background = Brushes.Black;	
				
				Field_2_1.Visibility = Visibility.Hidden;
				Field_2_2.Visibility = Visibility.Hidden;
				Field_2_3.Visibility = Visibility.Hidden;
				Field_2_4.Visibility = Visibility.Hidden;
				
				Vpr.Foreground = Brushes.White;
				Vpr.Background = Brushes.Black;
						
				blinkTimer.Stop();
				
			}
			else
			{
				Field_1_1.Visibility = Visibility.Visible;
				Field_1_2.Visibility = Visibility.Visible;
				Field_1_3.Visibility = Visibility.Visible;
				Field_1_4.Visibility = Visibility.Visible;
				ap.Foreground = Brushes.Black;
				ap.Background = Brushes.Lime;				
			}
			
			
		}
		
		void M4_Click(object sender, EventArgs e)
		{
			//Вкл/откл Vпр
			if (Field_2_1.Visibility == Visibility.Visible)
			{
				Field_2_1.Visibility = Visibility.Hidden;
				Field_2_2.Visibility = Visibility.Hidden;
				Field_2_3.Visibility = Visibility.Hidden;
				Field_2_4.Visibility = Visibility.Hidden;
				
				Vpr.Foreground = Brushes.White;
				Vpr.Background = Brushes.Black;
						
				blinkTimer.Stop();
				
			}
			else
			{
				if (Field_1_1.Visibility == Visibility.Visible)
				{
					Field_2_1.Visibility = Visibility.Visible;
					Field_2_2.Visibility = Visibility.Visible;
					Field_2_3.Visibility = Visibility.Visible;
					Field_2_4.Visibility = Visibility.Visible;
	
					blinkTimer.Start();
				}
			}			
		}
		
	}
}