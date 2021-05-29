// Góc pha của Solarduino 

// Tóm tắt Ghi chú 
// Lưu ý: An toàn là rất quan trọng khi xử lý điện. Chúng tôi không chịu trách nhiệm khi bạn tự chịu rủi ro. 
// Lưu ý: Mã Góc Pha này cần 2 sóng tín hiệu cùng tần số để hoạt động. Bạn có thể sử dụng mô-đun dòng điện AC và mô-đun điện áp AC. 
// Lưu ý: Code này giám sát độ lệch pha giữa 2 sóng và cả giá trị tần số của nó. 
// Lưu ý: Giá trị hiển thị trong Serial Monitor được làm mới mỗi giây, có thể được sử dụng cho 50Hz và 60Hz. 
// Lưu ý: Tần số được đo bằng thời gian đếm và lấy trung bình cho mỗi 50 mẫu được lấy (trong 50 hz) (1 mẫu là 1 chu kỳ). 
// Lưu ý: Việc hiệu chỉnh trước ápAnalogOffset & currentAnalogOffset là cần thiết để thực hiện theo cách thủ công.
// Lưu ý: Đơn vị cung cấp độ chính xác hợp lý và có thể không so sánh được với các sản phẩm thương mại và thương mại đắt tiền khác. 
// Lưu ý: Tất cả tín dụng sẽ được trao cho Solarduino. 
/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/////////////*/

 
 
        / * 0- Tổng quát * / 

        int decimalPrecision = 2;                  // vị trí thập phân cho tất cả các giá trị được hiển thị trong Màn hình LED & Màn hình nối tiếp 

        / * 1 - Đo góc pha, tần số và hệ số công suất * / 
        
        int expectedFrequency = 50;                 // Nhập tần số cho lưới điện chính (50/60 hz)
        int analogInputPin1PA = A2;                 // Chân đầu vào cho cảm biến analogRead1. Sử dụng cảm biến điện áp làm tài liệu tham khảo chính ở đây.
        int analogInputPin2PA = A3;                  // Chân đầu vào cho cảm biến analogRead2. Sử dụng dòng điện hoặc điện áp làm tham chiếu thứ cấp. 
        float voltageAnalogOffset =4;               // Đây là để bù đắp giá trị tương tự cho analogInput1 
        float currentAnalogOffset =-12;             // Đây là để bù đắp giá trị tương tự cho analogInput2
        unsigned long startMicrosPA;                / * bắt đầu đếm thời gian cho Góc pha và khoảng thời gian (tính bằng micro giây) * /
        unsigned long vCurrentMicrosPA;             / * thời gian hiện tại cho analogInput1 (điện áp) (tính bằng micro giây). AnalogInput1 được sử dụng để tham chiếu cho góc pha * /
        unsigned long iCurrentMicrosPA;             / * thời gian hiện tại cho analogInput2 (hiện tại / điện áp) (tính bằng micro giây). * /
        unsigned long periodMicrosPA;               / * thời gian hiện tại cho khoảng thời gian kỷ lục của sóng * /
        float vAnalogValue =0;                      / * là giá trị tương tự cho cảm biến điện áp / analogInput1 và trung tâm ở giá trị 0 * /
        float iAnalogValue =0;                      / * là giá trị tương tự cho cảm biến hiện tại / analogInput2 và trung tâm ở giá trị 0 * /
        float previousValueV =0;                    / * sử dụng để ghi lại giá trị đỉnh cho cảm biến điện áp * /
        float previousValueI =0;                    / * sử dụng để ghi lại giá trị đỉnh cho cảm biến hiện tại * /
        float previousphaseAngleSample=0;           / * đọc mẫu trước đó để thay thế giá trị sai dưới 100 micro giây * /
        float phaseAngleSample =0;                  / * là chênh lệch thời gian giữa 2 giá trị cảm biến (tính bằng micro giây) * /
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
        

void setup() 
{
  
        /* 0- General */
        
        Serial.begin(9600);
  
}

void loop() 
{


         / * 1 - Đo góc pha, tần số và hệ số công suất * / 
        
        vAnalogValue = analogRead(analogInputPin1PA)-512 + voltageAnalogOffset;        / * đọc analogInput1 với điều chỉnh trung tâm * /
        iAnalogValue = analogRead(analogInputPin2PA)-512 + currentAnalogOffset;       / * đọc analogInput2 với điều chỉnh trung tâm * /


        if((vAnalogValue>0) && a == 3)                                          / * giai đoạn bắt đầu đo ban đầu khi sóng analogInput1 lớn hơn 0 * / 
        {
          a=0;                                                                  / * cho phép chuyển sang giai đoạn tiếp theo * 
        }
        
        if((vAnalogValue<=0) && a ==0)                                          / * khi giá trị tương tự của analogInput1 nhỏ hơn hoặc bằng 0 * / 
        {
          startMicrosPA = micros();                                             / * bắt đầu đếm thời gian cho tất cả * / 
          a=1;                                                                 / * cho phép chuyển sang giai đoạn tiếp theo * / 
        }
        
        if((vAnalogValue>0) && a ==1)                                           / * khi giá trị tương tự của analogInput1 lớn hơn 0 * /
        {
          a = 2;                                                                / * cho phép chuyển sang giai đoạn tiếp theo * / 
          previousValueV = 0;                                                   / * giá trị đặt lại. Giá trị này được so sánh để đo giá trị đỉnh cho analogInput1 * /
          previousValueI = 0;                                                   / * giá trị đặt lại. Giá trị này được so sánh để đo giá trị đỉnh cho analogInput2 * / 
        }  
     
        if((vAnalogValue > previousValueV ) && a==2)                            / * ghi lại giá trị đo hiện tại thay thế giá trị đỉnh trước đó * / 
        {
          previousValueV = vAnalogValue ;                                       / * ghi lại thời gian hiện tại cho analogInput1 * / 
          vCurrentMicrosPA = micros();                                          / * ghi lại thời gian hiện tại cho analogInput1 * / 
        }
        
        if((iAnalogValue > previousValueI) && a==2)                             / * nếu giá trị đo hiện tại lớn hơn giá trị đỉnh trước đó của analogInput2 * /
        {
          previousValueI = iAnalogValue ;                                       / * ghi lại giá trị đo hiện tại thay thế giá trị đỉnh trước đó * / 
          iCurrentMicrosPA = micros();                                          / * ghi lại thời gian hiện tại cho analogInput2 * / 
        }
  
        if((vAnalogValue <=0) && a==2)                                          / * khi giá trị tương tự của analogInput1 nhỏ hơn hoặc bằng 0 * / 
        {
          periodMicrosPA = micros();                                             / * ghi lại thời gian hiện tại trong 1 khoảng thời gian * / 
          periodSample = periodMicrosPA - startMicrosPA;                        / * chu kỳ wave là thời gian hiện tại trừ đi thời gian bắt đầu (cho 1 mẫu) * / 
          periodSampleAccumulate = periodSampleAccumulate + periodSample;       / * cộng dồn hoặc cộng dồn thời gian cho tất cả các lần đọc mẫu của sóng chu kỳ * / 
          voltagePhaseAngle = vCurrentMicrosPA - startMicrosPA;                  / * thời gian thực hiện cho analogInput1 từ 0 (sóng xuống) đến giá trị đỉnh (sóng lên) * /
          currentPhaseAngle = iCurrentMicrosPA - startMicrosPA;                 / * thời gian được thực hiện cho analogInput2 từ 0 (sóng xuống) đến giá trị đỉnh (sóng lên) * / 
          phaseAngleSample = currentPhaseAngle - voltagePhaseAngle;             / * chênh lệch thời gian giữa giá trị đỉnh analogInput1 và giá trị đỉnh analogInput2 * / 
          if(phaseAngleSample>=100)                                             / * nếu chênh lệch thời gian hơn 100 micro giây * / 
          {
          previousphaseAngleSample = phaseAngleSample;                         / * thay thế giá trị trước đó bằng giá trị hiện tại mới * / 
          }
          if(phaseAngleSample<100)                                              / * if time chênh lệch ít hơn 100 micro giây (có thể là nhiễu hoặc giá trị giả) * /
          {
          phaseAngleSample = previousphaseAngleSample;                          / * lấy giá trị trước đó thay vì sử dụng giá trị thấp * / 
          }
          phaseAngleAccumulate = phaseAngleAccumulate + phaseAngleSample;       / * cộng dồn hoặc cộng dồn thời gian cho tất cả các lần đọc mẫu của chênh lệch thời gian * / 
          sampleCount = sampleCount + 1;                                        / * đếm số mẫu * /
          startMicrosPA = periodMicrosPA;                                       / * đặt lại thời gian bắt đầu * /
          a=1;                                                                 / * đặt lại chế độ sân khấu * / 
          previousValueV = 0;                                                   / * đặt lại giá trị đỉnh cho analogInput1 cho tập tiếp theo * /
          previousValueI = 0;                                                    / * đặt lại giá trị đỉnh cho analogInput2 cho tập hợp tiếp theo * / 
        }

        if(sampleCount == expectedFrequency)                                          / * nếu số lượng tổng số mẫu được ghi lại bằng 50 theo mặc định * / 
        {
          averagePeriod = periodSampleAccumulate/sampleCount;                         / * thời gian trung bình cho một khoảng thời gian của sóng từ tất cả các bài đọc mẫu * /
          frequency = 1000000 / averagePeriod;                                        / * giá trị tần số được tính toán * / 
          phaseDifference = phaseAngleAccumulate / sampleCount;                       / * chênh lệch thời gian trung bình giữa 2 giá trị đỉnh của cảm biến từ tất cả các kết quả đọc mẫu * / 
          phaseAngle = ((phaseDifference*360) / averagePeriod);                       / * góc pha được tính toán theo độ (trong số 360) * / 
          powerFactor = cos(phaseAngle*0.017453292);                                 / * hệ số công suất. Cos tính bằng radian, công thức bên trái đã chuyển đổi độ sang rad. * /  
          Serial.print("Phase Angle :");
          Serial.print(phaseAngle,decimalPrecision);
          Serial.print("Â°  ");
          Serial.print("Frequency :");
          Serial.print(frequency,decimalPrecision);
          Serial.print("Hz  ");
          Serial.print("Power Factor :");
          Serial.println(powerFactor,decimalPrecision);
          sampleCount = 0;                                                          / * đặt lại số lượng đếm mẫu * / 
          periodSampleAccumulate = 0;                                                / * đặt lại thời gian tích lũy cho sóng chu kỳ từ tất cả các mẫu * / 
          phaseAngleAccumulate =0;                                                    / * đặt lại thời gian tích lũy cho chênh lệch thời gian từ tất cả các mẫu * / 
        
        }

  
       

}
