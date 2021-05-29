// Góc pha của Solarduino 

// Tóm tắt 
// lưu ý // Lưu ý: An toàn là rất quan trọng khi xử lý điện. Chúng tôi không chịu trách nhiệm khi bạn tự chịu rủi ro. 
// Lưu ý: Mã Góc Pha này cần 2 sóng tín hiệu cùng tần số để hoạt động. Bạn có thể sử dụng mô-đun dòng điện AC và mô-đun điện áp AC. 
// Lưu ý: Code này giám sát độ lệch pha giữa 2 sóng và cả giá trị tần số của nó. 
// Lưu ý: Giá trị hiển thị trong Màn hình nối tiếp và Màn hình LCD được làm mới mỗi giây, có thể được sử dụng cho 50Hz và 60Hz. 
// Lưu ý: Tần số được đo bằng thời gian đếm và lấy trung bình cho mỗi 50 mẫu được lấy (trong 50 hz) (1 mẫu là 1 chu kỳ).
// Lưu ý: Việc hiệu chỉnh trước điện ápAnalogOffset & currentAnalogOffset được tự động thực hiện khi bạn nhấn nút CHỌN của Tấm chắn Màn hình LCD. 
// Lưu ý: Đơn vị cung cấp độ chính xác hợp lý và có thể không so sánh được với các sản phẩm thương mại và thương mại đắt tiền khác. 
// Lưu ý: Tất cả tín dụng sẽ được trao cho Solarduino. 
/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/////////////*/

 
 
        / * 0- Tổng quát * / 


        int decimalPrecision = 2;                   // vị trí thập phân cho tất cả các giá trị được hiển thị trong Màn hình LED & Màn hình nối tiếp 


        / * 1 - Đo góc pha, tần số và hệ số công suất * /

        int expectedFrequency = 50;                 //Nhập tần số cho lưới chính ( 50/60 hz)
        int analogInputPin1PA = A2;                 // Chân đầu vào cho cảm biến analogRead1. Sử dụng cảm biến điện áp làm tài liệu tham khảo chính ở đây. 
        int analogInputPin2PA = A3;                 // Chân đầu vào cho cảm biến analogRead2. Sử dụng dòng điện hoặc điện áp làm tham chiếu thứ cấp. 
        float voltageAnalogOffset =0;               // Đây là để bù đắp giá trị tương tự cho analogInput1 
        float currentAnalogOffset =0;               // Đây là để bù đắp giá trị tương tự cho analogInput2 
        unsigned long startMicrosPA;                / * bắt đầu đếm thời gian cho Góc pha và khoảng thời gian (tính bằng micro giây) * /
        unsigned long vCurrentMicrosPA;             / * thời gian hiện tại cho analogInput1 (điện áp) (tính bằng micro giây). AnalogInput1 được sử dụng để tham chiếu cho góc pha * /
        unsigned long iCurrentMicrosPA;             / * thời gian hiện tại cho analogInput2 (hiện tại / điện áp) (tính bằng micro giây). * /
        unsigned long periodMicrosPA;              / * thời gian hiện tại cho khoảng thời gian kỷ lục của sóng * /
        float vAnalogValue =0;                      / * là giá trị tương tự cho cảm biến điện áp / analogInput1 và trung tâm ở giá trị 0 * /
        float iAnalogValue =0;                      / * là giá trị tương tự cho cảm biến hiện tại / analogInput2 và trung tâm ở giá trị 0 * /
        float previousValueV =0;                    / * sử dụng để ghi lại giá trị đỉnh cho cảm biến điện áp * /
        float previousValueI =0;                    / * sử dụng để ghi lại giá trị đỉnh cho cảm biến hiện tại * /
        float previousphaseAngleSample=0;           / * đọc mẫu trước đó để thay thế giá trị sai dưới 100 micro giây * /
        float phaseAngleSample =0;                 / * là chênh lệch thời gian giữa 2 giá trị cảm biến (tính bằng micro giây) * /
        float phaseAngleAccumulate =0;              / * là chênh lệch thời gian tích lũy cho các mẫu tích lũy * /
        float periodSample=0;                       / * là chênh lệch thời gian cho một khoảng thời gian của sóng đối với một mẫu (tính bằng micro giây) * /
        float periodSampleAccumulate = 0;           / * là chênh lệch thời gian tích lũy cho các mẫu tích lũy * /
        float phaseDifference =0;                   / * là tập hợp chênh lệch thời gian trung bình của 2 cảm biến * /
        float phaseAngle =0;                        / * là góc pha theo độ (trong số 360) * /
        float frequency = 0;                        / * là tần số của sóng cảm biến điện áp * /
        float voltagePhaseAngle=0;                  / * là thời gian được ghi lại từ lúc bắt đầu để đạt đến giá trị đỉnh cho analogInput1 tính bằng micro giây * /
        float currentPhaseAngle=0;                  / * là thời gian được ghi lại từ lúc bắt đầu để đạt đến giá trị cao nhất cho analogInput2 tính bằng micro giây * /
        float averagePeriod =0;                     / * là tập hợp thời gian trung bình được ghi lại trong một khoảng thời gian của sóng * /
        int sampleCount = 0;                        / * để đếm xem có bao nhiêu bộ mẫu * /
        int a = 3;                                  / * sử dụng cho hoạt động chuyển mạch * /
        float powerFactor;                          / * để tính toán hệ số công suất * /


            / * 1.1 - Chênh lệch góc pha * /

            float currentOffsetRead =0;
            float currentOffsetLastSample =0;
            float currentOffsetSampleCount=0;
            float voltageOffsetRead =0;
            float voltageOffsetLastSample =0;
            float voltageOffsetSampleCount=0;

 
        /* 2 - LCD Display  */

        #include<LiquidCrystal.h>                   / * Tải Thư viện tinh thể lỏng (theo mặc định đã được tích hợp sẵn với phần mềm giải pháp arduino) * /
        LiquidCrystal LCD(8,9,4,5,6,7);             / * Tạo đối tượng LiquidCrystal có tên là LCD. Chốt có thể thay đổi tùy theo mô-đun LCD mà bạn sử dụng * /
      

void setup() 
{
  
        / * 0- Chung * /
        
        Serial.begin(9600);

        /* 2 - LCD Display  */

        LCD.begin(16,2);                                  / * Nói với Arduino rằng màn hình LCD của chúng ta có 16 cột và 2 hàng * /
        LCD.setCursor(0,0);                               / * Đặt màn hình LCD bắt đầu với góc trên bên trái của màn hình * / 
}

void loop() 
{

        / * 0- Chung * /


             / * 0.1- Chức năng nút * /
        
              int buttonRead;
              buttonRead = analogRead (0);                                                        // Đọc chân analog A0. Chân A0 tự động được gán cho chức năng Nút hiển thị LCD (không thể thay đổi)

              / * Nút bên phải được nhấn * /
              if (buttonRead < 60) 
              {   LCD.setCursor(0,0); LCD.print ("PRESS <SELECT>   "); }       
     
              / * Nhấn nút Lên * /
              else if (buttonRead < 200) 
              {   LCD.setCursor(0,0); LCD.print ("PRESS <SELECT>   "); }   
                 
              / * Nút xuống được nhấn * /
              else if (buttonRead < 400)
              {   LCD.setCursor(0,0); LCD.print ("PRESS <SELECT>   "); }        
     
               /* Left button is pressed */
              else if (buttonRead < 600)
              {   LCD.setCursor(0,0); LCD.print ("PRESS <SELECT>   "); } 
     
              / * Nút chọn được nhấn * /
              else if (buttonRead < 800)
              {   
              currentOffsetRead = 1;                                                              // để kích hoạt bù đắp cho hiện tại
              voltageOffsetRead = 1;                                                              // để kích hoạt bù đắp cho điện áp
              LCD.setCursor(0,0);                                                                 * đặt các từ hiển thị bắt đầu từ góc trên bên trái * /
              LCD.print ("INITIALIZING..... ");
              LCD.setCursor(0,1);                                                                 / * đặt các từ hiển thị bắt đầu từ góc dưới bên trái * /
              LCD.print ("WAIT 5 SEC ..... ");
              }


        /* 1 - Phase Angle, Frequency and Power Factor measurement */
        
        vAnalogValue = analogRead(analogInputPin1PA)-512 + voltageAnalogOffset;       /* read analogInput1 with center adjusted*/
        iAnalogValue = analogRead(analogInputPin2PA)-512 + currentAnalogOffset;       /* read analogInput2 with center adjusted*/

        if((vAnalogValue>0) && a == 3)                                          /* initial begining stage of measurement when analogInput1 wave larger than 0 */
        {
          a=0;                                                                  /* allow to change to the next stage */
        }
        
        if((vAnalogValue<=0) && a ==0)                                          /* when analog value of analogInput1 smaller or equal than 0*/
        {
          startMicrosPA = micros();                                             /* start counting time for all*/
          a=1;                                                                  /* allow to change to the next stage */
        }
        
        if((vAnalogValue>0) && a ==1)                                           /* when analog value of analogInput1 larger than 0*/
        {
          a = 2;                                                                /* allow to change to the next stage */
          previousValueV = 0;                                                   /* reset value. This value to be compared to measure peak value for analogInput1 */
          previousValueI = 0;                                                   /* reset value. This value to be compared to measure peak value for analogInput2 */
        }  
     
        if((vAnalogValue > previousValueV ) && a==2)                            /* if current measured value larger than previous peak value of analogInput1 */
        {
          previousValueV = vAnalogValue ;                                       /* record current measure value replace previous peak value */
          vCurrentMicrosPA = micros();                                          /* record current time for analogInput1 */
        }
        
        if((iAnalogValue > previousValueI) && a==2)                             /* if current measured value larger than previous peak value of analogInput2 */
        {
          previousValueI = iAnalogValue ;                                       /* record current measure value replace previous peak value */
          iCurrentMicrosPA = micros();                                          /* record current time for analogInput2 */
        }
  
        if((vAnalogValue <=0) && a==2)                                          /* when analog value of analogInput1 smaller or equal than 0*/
        {
          periodMicrosPA = micros();                                            /* record current time for 1 period */
          periodSample = periodMicrosPA - startMicrosPA;                        /* period wave is the current time minus the starting time (for 1 sample)*/
          periodSampleAccumulate = periodSampleAccumulate + periodSample;       /* accumulate or add up time for all sample readings of period wave */
          voltagePhaseAngle = vCurrentMicrosPA - startMicrosPA;                 /* time taken for analogInput1 from 0 (down wave) to peak value (up wave)*/
          currentPhaseAngle = iCurrentMicrosPA - startMicrosPA;                 /* time taken for analogInput2 from 0 (down wave) to peak value (up wave)*/
          phaseAngleSample = currentPhaseAngle - voltagePhaseAngle;             /* time difference between analogInput1 peak value and analogInput2 peak value*/
          if(phaseAngleSample>=100)                                             /* if time difference more than 100 micro seconds*/
          {
          previousphaseAngleSample = phaseAngleSample;                          /* replace previous value using new current value */ 
          }
          if(phaseAngleSample<100)                                              /* if time difference less than 100 micro seconds (might be noise or fake values)*/
          {
          phaseAngleSample = previousphaseAngleSample;                          /* take previous value instead using low value*/
          }
          phaseAngleAccumulate = phaseAngleAccumulate + phaseAngleSample;       /* accumulate or add up time for all sample readings of time difference */
          sampleCount = sampleCount + 1;                                        /* count sample number */
          startMicrosPA = periodMicrosPA;                                       /* reset begining time */
          a=1;                                                                  /* reset stage mode */
          previousValueV = 0;                                                   /* reset peak value for analogInput1 for next set */
          previousValueI = 0;                                                   /* reset peak value for analogInput2 for next set */
        }

        if(sampleCount == expectedFrequency)                                          /* if number of total sample recorded equal 50 by default */
        {
          averagePeriod = periodSampleAccumulate/sampleCount;                         /* average time for a period of wave from all the sample readings*/
          frequency = 1000000 / averagePeriod;                                        /* the calculated frequency value */
          phaseDifference = phaseAngleAccumulate / sampleCount;                       /* average time difference between 2 sensor peak values from all the sample readings */
          phaseAngle = ((phaseDifference*360) / averagePeriod);                       /* the calculated phase angle in degree (out of 360)*/
          powerFactor = cos(phaseAngle*0.017453292);                                  /* power factor. Cos is in radian, the formula on the left has converted the degree to rad. */
          Serial.print("Phase Angle :");
          Serial.print(phaseAngle,decimalPrecision);
          Serial.print("Â°  ");
          Serial.print("Frequency :");
          Serial.print(frequency,decimalPrecision);
          Serial.print("Hz  ");
          Serial.print("Power Factor :");
          Serial.println(powerFactor,decimalPrecision);
          sampleCount = 0;                                                            /* reset the sample counting quantity */
          periodSampleAccumulate = 0;                                                 /* reset the accumulated time for period wave from all samples */
          phaseAngleAccumulate =0;                                                    /* reset the accumulated time for time difference from all samples*/
        
          /* 2 - LCD Display  */
        
          LCD.setCursor(0,0);                                                       /* Set cursor to first colum 0 and second row 1  */
          LCD.print("P. Angle =");
          LCD.print(phaseAngle,decimalPrecision);                                   /* display current value in LCD in first row  */
          LCD.print((char)223);
          LCD.print("    ");
          LCD.setCursor(0,1); 
          LCD.print(frequency,decimalPrecision);
          LCD.print("Hz  ");
          LCD.print(powerFactor,decimalPrecision);                                  /* display current value in LCD in first row  */
          LCD.print("PF    ");
        
        }


            /* 1.1 - Phase Angle Offset*/

            if( voltageOffsetRead == 1)
              {
                voltageAnalogOffset = 0; 
                if(millis()>= voltageOffsetLastSample + 1)                                        /* keep countng time for offset1*/
                    {                                                                            
                      voltageOffsetSampleCount = voltageOffsetSampleCount + 1;                    /* 1 milli second add 1 count*/                                                      
                      voltageOffsetLastSample = millis();                                         /* to reset the time again so that next cycle can start again*/                                  
                    }                                                                                 
                  if(voltageOffsetSampleCount == 1500)                                            /* after 2 seconds, run this codes.  */
                    {    
                      vAnalogValue = analogRead(analogInputPin1PA)-512 + voltageAnalogOffset;     /* read analogInput1 with center adjusted*/                                                                        
                      voltageAnalogOffset = -1*(vAnalogValue);                                    /* set the offset values */
                      voltageOffsetRead = 0;                                                      /* go for second offset Settings */                     
                      voltageOffsetSampleCount = 0;                                               /* to reset the time again so that next cycle can start again */                                                             
                    } 
              }   
            
            if( currentOffsetRead == 1)
              {
                currentAnalogOffset = 0; 
                if(millis()>= currentOffsetLastSample + 1)                                        /* keep countng time for offset1*/
                    {                                                                            
                      currentOffsetSampleCount = currentOffsetSampleCount + 1;                    /* 1 milli second add 1 count*/                                                      
                      currentOffsetLastSample = millis();                                         /* to reset the time again so that next cycle can start again*/                                  
                    }                                                                                 
                  if(currentOffsetSampleCount == 1500)                                            /* after 2 seconds, run this codes.  */
                    {     
                      iAnalogValue = analogRead(analogInputPin2PA)-512 + currentAnalogOffset;     /* read analogInput2 with center adjusted*/                                                                       
                      currentAnalogOffset = -1*(iAnalogValue);                                    /* set the offset values */
                      currentOffsetRead = 0;                                                      /* go for second offset Settings */                     
                      currentOffsetSampleCount = 0;                                               /* to reset the time again so that next cycle can start again */                                                             
                    } 
              }   

}
