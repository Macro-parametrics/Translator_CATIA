using System;

namespace CATIATranslator
{
    class Tool
    {
        // 반올림 함수
        static public void Round(object[] coords)
        {
            int digit = 3; // 소숫점 셋 째 자리에서 반올림

            for (int i = 0; i < coords.Length; i++)
            {
                double value = (double)coords.GetValue(i);
                coords.SetValue(Math.Round(value, digit), i);

                if (Math.Abs(value) < 0.0001)
                    coords.SetValue(0.0, i);
            }
        }
    }
}
