/*
 * rtc.c
 *
 *  Created on: 22 févr. 2013
 *      Author			: 	duhautf
 */

#include "include.h"

extern void usb_cpu_WdogRefresh (void);

ulong TimeRTC;

uint PreviousHour;

uint32_t FirstStart = 0;

/**
 * @brief  Manage RTC in chrono
 * @param  None
 * @retval None
 */

void RTC_Chrono (void)
{
	uint32_t Correction = 0;

	if ((State.TimerSec == 3) && (RTCInit == 0))
		Init_RTC();


	/* Daylight */
	if ((!(RTCInit == 0)) && (FirstStart == 0))
	{
		FirstStart = 1;

		if (CalculateHours(&Status.NextDaylight) <= CalculateHours(&State.DateR))
		{
			/* Do action */	

			switch (Menu.Daylight)
			{
			case EUROPE:
			case USCANADA:

				if (Status.NextDaylight.Month == 3)
				{
					State.DateR.Hr++;		/**/
					Correction = 0;
				}
				else
				{
					State.DateR.Hr--;		/**/
					Correction = 1;
				}
				break;	

			case AUSTRALIA:
				if (Status.NextDaylight.Month == 4)
				{
					State.DateR.Hr--;		/**/
					Correction = 1;
				}
				else
				{
					State.DateR.Hr++;		/**/
					Correction = 0;
				}
				break;									
			}
			State.DateW.Sec = State.DateR.Sec;
			State.DateW.Min = State.DateR.Min;
			State.DateW.Hr = State.DateR.Hr;
			State.DateW.Date = State.DateR.Date;
			State.DateW.Month = State.DateR.Month;
			State.DateW.Year = State.DateR.Year;

			Write_RTC(Correction);
		}
	}


	if (RtcWrite != 0)
	{
		RtcWriteOff;
		Write_RTC(0);
	}
}

/**
 * @brief  The RTC initialisation function. It configures the RTC to generate
 *				  an interrupt every second. The alarm can be also configured to
 *				  trigger at a specified time.
 * @param  None
 * @retval None
 */
void Init_RTC(void)
{
	/* Protection off */
	SYSTEM.PRCR.WORD = 0xA503;		

	TimeRTC = State.TimerSec + 4L;

	/* Check if the MCU has come from a cold start (power on reset) */
	if(0 == SYSTEM.RSTSR1.BIT.CWSF)
	{	
		/* Set the warm start flag */
		SYSTEM.RSTSR1.BIT.CWSF = 1;

		/* Disable the sub-clock oscillator */
		SYSTEM.SOSCCR.BIT.SOSTP = 1;

		/* Wait for register modification to complete */
		while((1 != SYSTEM.SOSCCR.BIT.SOSTP) && (TimeRTC < State.TimerSec))
		{
			usb_cpu_WdogRefresh();
		}

		/* Disable the input from the sub-clock */
		RTC.RCR3.BYTE = 0x0C;	


		/* Wait for the register modification to complete */
		while((0 != RTC.RCR3.BIT.RTCEN) && (TimeRTC < State.TimerSec))
		{
			usb_cpu_WdogRefresh();
		}

		/* Wait for at least 5 cycles of sub-clock */
		uint32_t wait = 0x6000;
		while(wait--);

		/* Start sub-clock */
		SYSTEM.SOSCCR.BIT.SOSTP = 0;
	}
	else
	{
		/* Start sub-clock */
		SYSTEM.SOSCCR.BIT.SOSTP = 0;	

		/* Wait for the register modification to complete */
		while((0 != SYSTEM.SOSCCR.BIT.SOSTP) && (TimeRTC < State.TimerSec))
		{
			usb_cpu_WdogRefresh();
		}
	}	

	/* Set RTC clock input from sub-clock, and supply to RTC module */
	RTC.RCR4.BIT.RCKSEL = 0;	
	RTC.RCR3.BIT.RTCEN = 1;	

	/* Wait for at least 5 cycles of sub-clock */
	uint32_t wait = 0x6000;
	while(wait--);

	/* It is now safe to set the RTC registers */

	/* Write 0 to RTC start bit */
	RTC.RCR2.BIT.START = 0x0;

	/* Wait for start bit to clear */
	while((0 != RTC.RCR2.BIT.START)  && (TimeRTC < State.TimerSec))
	{
		usb_cpu_WdogRefresh();
	}

	/* Alarm enable bits are undefined after a reset,
	   disable non-required alarm features */
	RTC.RWKAR.BIT.ENB = 0;
	RTC.RDAYAR.BIT.ENB = 0;
	RTC.RMONAR.BIT.ENB = 0;
	RTC.RYRAREN.BIT.ENB = 0;

	/* Enable RTC Alarm interrupts */	
	//IPR(RTC, ALM) = 7u;
	//IEN(RTC, ALM) = 1u;
	//IR(RTC, ALM)  = 0u;

	/* Enable auto adjust time*/
	RTC.RCR2.BIT.AADJE = 0x1;		// auto adjust mode
	RTC.RADJ.BIT.PMADJ = 0x2;		// 0x01 -> add    0x02 -> substract
	RTC.RADJ.BIT.ADJ = 60;			//

	/* Enable RTC Periodic interrupts */	
	IPR(RTC, PRD) = 6u;
	IEN(RTC, PRD) = 1u;
	IR(RTC, PRD)  = 0u;

	/* Enable alarm and periodic interrupts,
	generate periodic interrupts every 1 second */
	RTC.RCR1.BYTE = 0xE5;

	/* Start the clock */
	RTC.RCR2.BIT.START = 0x1;

	/* Wait until the start bit is set to 1 */
	while((1 != RTC.RCR2.BIT.START) && (TimeRTC < State.TimerSec))
	{
		usb_cpu_WdogRefresh();
	}

	if (TimeRTC < State.TimerSec)
		RtcDefOn;
	else
		RtcDefOff;

	State.DateR.Sec = BcdUchar(RTC.RSECCNT.BYTE & 0x0000007F);
	State.DateR.Min = BcdUchar(RTC.RMINCNT.BYTE & 0x0000007F);
	State.DateR.Hr = BcdUchar(RTC.RHRCNT.BYTE  & 0x0000003F);

	State.DateR.Date = BcdUchar(RTC.RDAYCNT.BYTE  & 0x0000003F);
	State.DateR.Month = BcdUchar(RTC.RMONCNT.BYTE & 0x0000007F);
	State.DateR.Year = 2000 + BcdUchar(RTC.RYRCNT.WORD & 0x0000007F);

	if (State.DateR.Year == 2000)
	{
		/* Configure the clock 	*/
		State.DateW.Sec = 0;
		State.DateW.Min = 0;
		State.DateW.Hr = 0;

		/* Configure the date */
		State.DateW.Date = 1;
		State.DateW.Month = 1;
		State.DateW.Year = 2014;
		RtcWriteOn;
	}

	PreviousHour = State.DateR.Hr;

	RTCInitOn;

}

/**
 * @brief  RTC alarm interrupt function generated when the time set in
 * 	  	the alarm registers matches with the current RTC time.
 * @param  None
 * @retval None
 */
void Excep_RTC_ALM(void)
{
	/* the alarm is triggered */

	/* Clear the interrupt flag */
	//IR(RTC, ALM) = 0;
}

/**
 * @brief  RTC periodic interrupt handler generated every 1 sec. It is
 *		  	used to update the State Date/Time.
 * @param  None
 * @retval None
 */


void Excep_RTC_SLEEP(void)
{
	uint32_t Correction = 0; 

	State.DateR.Sec = BcdUchar(RTC.RSECCNT.BYTE & 0x0000007F);
	State.DateR.Min = BcdUchar(RTC.RMINCNT.BYTE & 0x0000007F);
	State.DateR.Hr = BcdUchar(RTC.RHRCNT.BYTE  & 0x0000003F);

	State.DateR.Date = BcdUchar(RTC.RDAYCNT.BYTE  & 0x0000003F);
	State.DateR.Month = BcdUchar(RTC.RMONCNT.BYTE & 0x0000007F);
	State.DateR.Year = 2000 + BcdUchar(RTC.RYRCNT.WORD & 0x0000007F);

	/* Means Enabled */ 
	if ((Menu.Daylight > 0) /*&& ()*/)
	{
		/* Check when hour changed */
		if (State.DateR.Hr != PreviousHour)
		{
			if ((Status.NextDaylight.Sec <= State.DateR.Sec) &&
					(Status.NextDaylight.Min <= State.DateR.Min) &&
					(Status.NextDaylight.Hr <= State.DateR.Hr) &&
					(Status.NextDaylight.Date <= State.DateR.Date) &&
					(Status.NextDaylight.Month <= State.DateR.Month) &&
					(Status.NextDaylight.Year <= State.DateR.Year))
			{
				/* Do action */	

				switch (Menu.Daylight)
				{
				case EUROPE:
				case USCANADA:

					if (Status.NextDaylight.Month == 3)
					{
						State.DateR.Hr++;		/**/
						Correction = 0;
					}
					else
					{
						State.DateR.Hr--;		/**/
						Correction = 1;
					}
					break;	

				case AUSTRALIA:
					if (Status.NextDaylight.Month == 4)
					{
						State.DateR.Hr--;		/**/
						Correction = 1;
					}
					else
					{
						State.DateR.Hr++;		/**/
						Correction = 0;
					}
					break;									
				}
				State.DateW.Sec = State.DateR.Sec;
				State.DateW.Min = State.DateR.Min;
				State.DateW.Hr = State.DateR.Hr;
				State.DateW.Date = State.DateR.Date;
				State.DateW.Month = State.DateR.Month;
				State.DateW.Year = State.DateR.Year;
				Write_RTC(Correction);
			}
			PreviousHour = State.DateR.Hr;
		}
	}

	if ((Menu.EqualPeriod & (1 << (DayOfWeek(&State.DateR) - 1))) != 0)
	{
		PeriodEqualOn;
	}
	else
	{
		PeriodEqualOff;
	}
}

/**
 * @brief  Write RTC register
 * @param  None
 * @retval None
 */
void Write_RTC (uint32_t ct)
{
	/* Reset the RTC unit */
	RTC.RCR2.BIT.RESET = 1;

	/* Wait until reset is complete */
	while(RTC.RCR2.BIT.RESET)
	{
		usb_cpu_WdogRefresh();
	}

	/* Write 0 to RTC start bit */
	RTC.RCR2.BIT.START = 0x0;

	/* Wait for start bit to clear */
	while(0 != RTC.RCR2.BIT.START)
	{
		usb_cpu_WdogRefresh();
	}

	/* Operate RTC in 24-hr mode */
	RTC.RCR2.BIT.HR24 = 0x1;

	/* Configure the clock 	*/
	RTC.RSECCNT.BYTE = UcharBcd (State.DateW.Sec);
	RTC.RMINCNT.BYTE = UcharBcd (State.DateW.Min);
	RTC.RHRCNT.BYTE = UcharBcd (State.DateW.Hr);

	/* Configure the date */
	RTC.RDAYCNT.BYTE = UcharBcd (State.DateW.Date);
	RTC.RMONCNT.BYTE = UcharBcd (State.DateW.Month);
	RTC.RYRCNT.WORD = UcharBcd (State.DateW.Year % 100);

	/* Configure the alarm as follows -
		Alarm time - 12:00:00
		Enable the hour, minutes and seconds alarm		*/
	RTC.RSECAR.BYTE = 0x80;
	RTC.RMINAR.BYTE = 0x80;
	RTC.RHRAR.BYTE = 0xD2;

	/* Alarm enable bits are undefined after a reset,
	   disable non-required alarm features */
	RTC.RWKAR.BIT.ENB = 0;
	RTC.RDAYAR.BIT.ENB = 0;
	RTC.RMONAR.BIT.ENB = 0;
	RTC.RYRAREN.BIT.ENB = 0;

	/* Enable alarm and periodic interrupts,
	generate periodic interrupts every 1 second */
	RTC.RCR1.BYTE = 0xE5;

	/* Verify RCR1 register write */
	while(0xE5 != RTC.RCR1.BYTE)
	{
		usb_cpu_WdogRefresh();
	}
	Init_RTC();

	/**/
	CalcNextDayLight(ct);	
}

/**
 * @brief  Convert BCD value to uchar
 * @param  value to convert
 * @retval None
 */
uchar BcdUchar(uchar c)
{
	return (c >> 4) * 10 + (c & 0x0F);
}


/**
 * @brief  Convert uchar value to BCD
 * @param  value to convert
 * @retval None
 */
uchar UcharBcd(uchar c)
{
	return (c / 10) * 16 + (c % 10);
}

/**
 * @brief  Zeller formula with ISO return Gregorian calendar
 * @param  StructDateTime
 * @retval 1 = Monday, 7 = Sunday
 */
uchar DayOfWeek(StructDateTime *dt)
{
	/* For the formula : 0 = sunday */
	/* For ISO : 1 = Monday, 7 = Sunday */
	uint i;
	i = (*dt).Date;                         /* Day : 1 to 31 */
	if ((*dt).Month <= 2)
	{
		i = i + (((*dt).Month + 13) * 26) /10;/* Month for January and February */
		i = i + ((*dt).Year - 1) % 100;       /* Year of the century - 1 */
		i = i + ((((*dt).Year - 1) % 100) / 4);
		i = i + (((*dt).Year - 1) / 100) / 4;
		i = i + 5 * (((*dt).Year - 1) / 100); /* Century */
	}
	else
	{
		i = i + (((*dt).Month + 1) * 26) /10; /* Month for others */
		i = i + (*dt).Year % 100;             /* Year of the century */
		i = i + (((*dt).Year % 100) / 4);
		i = i + ((*dt).Year / 100) / 4;
		i = i + 5 * ((*dt).Year / 100);       /* Century */
	}
	return  ((i + 5) % 7) + 1;              /* Modulo and ISO format */
}


uint32_t CalculateHours (StructDateTime *dt)
{
	uint32_t m,y,d,h;

	m = (*dt).Month;
	y = (*dt).Year;
	d = (*dt).Date;
	h = (*dt).Hr;

	m = (m + 9) % 12;
	y = y - m/10;
	return ((365*y + y/4 - y/100 + y/400 + (m*306 + 5)/10 + ( d - 1 )) * 24) + h	;
}


void CalcDayLights(StructDateTime *pt1,StructDateTime *pt2)
{
	uint32_t i;

	switch (Menu.Daylight)
	{
	case EUROPE:
		(*pt1).Sec = 0;
		(*pt1).Min = 0;
		(*pt1).Hr = 2;
		(*pt1).Month = 3;
		(*pt1).Date = 25;

		(*pt2).Sec = 0;
		(*pt2).Min = 0;
		(*pt2).Hr = 3;
		(*pt2).Month = 10;
		(*pt2).Date = 25;
		break;
	case USCANADA:
		(*pt1).Sec = 0;
		(*pt1).Min = 0;
		(*pt1).Hr = 2;
		(*pt1).Month = 3;
		(*pt1).Date = 8;

		(*pt2).Sec = 0;
		(*pt2).Min = 0;
		(*pt2).Hr = 2;
		(*pt2).Month = 11;
		(*pt2).Date = 1;
		break;
	case AUSTRALIA:
		(*pt1).Sec = 0;
		(*pt1).Min = 0;
		(*pt1).Hr = 3;
		(*pt1).Month = 4;
		(*pt1).Date = 1;

		(*pt2).Sec = 0;
		(*pt2).Min = 0;
		(*pt2).Hr = 2;
		(*pt2).Month = 10;
		(*pt2).Date = 1;
		break;				
	}

	for (i = 0;i < 7;i++)
	{
		if (DayOfWeek(pt1) == 7)
		{
			break;	
		}
		(*pt1).Date ++;		
	}

	for (i = 0;i < 7;i++)
	{
		if (DayOfWeek(pt2) == 7)
		{
			break;	
		}	
		(*pt2).Date ++;	
	}	
}

void CalcNextDayLight(uint32_t ct)
{
	/* Save in Status changes, Menu saved in lcdmenu,  */	
	uint32_t FirstDayLightHours,SecondDayLightHours,ActualHours;

	StructDateTime FirstDayLight;
	StructDateTime SecondDayLight;

	//DISABLE_IRQ();
	FirstDayLight.Year = State.DateR.Year ;	
	//ENABLE_IRQ();
	SecondDayLight.Year = FirstDayLight.Year;

	if (Menu.Daylight != 0)
	{
		/*Marcius utols� vas�rnap okt�ber utols� vas�rnap*/

		CalcDayLights(&FirstDayLight,&SecondDayLight);
		FirstDayLightHours = CalculateHours(&FirstDayLight);
		SecondDayLightHours = CalculateHours(&SecondDayLight);
		ActualHours = CalculateHours(&State.DateR);

		if (ct == 1)
		{
			ActualHours += 2;
		}

		if (ActualHours < FirstDayLightHours)
		{
			/* Below first daylight*/
			Status.NextDaylight.Sec = FirstDayLight.Sec;
			Status.NextDaylight.Min = FirstDayLight.Min;
			Status.NextDaylight.Hr = FirstDayLight.Hr;
			Status.NextDaylight.Date = FirstDayLight.Date;
			Status.NextDaylight.Month = FirstDayLight.Month;
			Status.NextDaylight.Year = FirstDayLight.Year;						

		}
		else  if (ActualHours > SecondDayLightHours)
		{	
			/* After second daylight*/
			/*Next year*/
			/*Marcius utols� vas�rnap okt�ber utols� vas�rnap*/

			FirstDayLight.Year++;
			CalcDayLights(&FirstDayLight,&SecondDayLight);

			Status.NextDaylight.Sec = FirstDayLight.Sec;
			Status.NextDaylight.Min = FirstDayLight.Min;
			Status.NextDaylight.Hr = FirstDayLight.Hr;
			Status.NextDaylight.Date = FirstDayLight.Date;
			Status.NextDaylight.Month = FirstDayLight.Month;
			Status.NextDaylight.Year = FirstDayLight.Year;					
		}
		else
		{
			/* Between two  */
			Status.NextDaylight.Sec = SecondDayLight.Sec;
			Status.NextDaylight.Min = SecondDayLight.Min;
			Status.NextDaylight.Hr = SecondDayLight.Hr;
			Status.NextDaylight.Date = SecondDayLight.Date;
			Status.NextDaylight.Month = SecondDayLight.Month;
			Status.NextDaylight.Year = SecondDayLight.Year;					
		}
	}
	else
	{
		Status.NextDaylight.Sec = 0;
		Status.NextDaylight.Min = 0;
		Status.NextDaylight.Hr = 0;
		Status.NextDaylight.Date = 0;
		Status.NextDaylight.Month = 0;
		Status.NextDaylight.Year = 0;		
	}

	StatusWriteOn;
}

