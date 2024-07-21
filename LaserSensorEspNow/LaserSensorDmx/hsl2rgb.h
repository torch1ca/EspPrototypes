
    void rgb2hsl(int *col)
    {
        float r = col[0] / 255.0f;
        float g = col[1] / 255.0f;
        float b = col[2] / 255.0f;
        float h, s, l = 0.0f;

        float maxVal = max(r, max(g, b));

        if (maxVal <= 0)
        {
            return;
        }

        float minVal = min(r, min(g, b));
        float dif = maxVal - minVal;

        if (maxVal > minVal)
        {
            if (g == maxVal)
            {
                h = (b - r) / dif * 60.0f + 120.0f;
            }
            else if (b == maxVal)
            {
                h = (r - g) / dif * 60.0f + 240.0f;
            }
            else if (b > g)
            {
                h = (g - b) / dif * 60.0f + 360.0f;
            }
            else
            {
                h = (g - b) / dif * 60.0f;
            }
            if (h < 0)
            {
                h = h + 360.0f;
            }
        }
        else
        {
            h = 0;
        }

        h *= 1.0f / 360.0f;
        s = (dif / maxVal) * 1.0f;
        l = maxVal;

        col[0] = h * 255.0f;
        col[1] = s * 255.0f;
        col[2] = l * 255.0f;
    }

    void hsl2rgb(int *col)
    {
        float h = col[0] / 255.0f;
        float s = col[1] / 255.0f;
        float l = col[2] / 255.0f;

        float r = l;
        float g = l;
        float b = l;

        if (s != 0)
        {
            float maxVal = l;
            float dif = l * s;
            float minVal = l - dif;

            h *= 360.0f;

            if (h < 60.0f)
            {
                r = maxVal;
                g = h * dif / 60.0f + minVal;
                b = minVal;
            }
            else if (h < 120.0f)
            {
                r = -(h - 120.0f) * dif / 60.0f + minVal;
                g = maxVal;
                b = minVal;
            }
            else if (h < 180.0f)
            {
                r = minVal;
                g = maxVal;
                b = (h - 120.0f) * dif / 60.0f + minVal;
            }
            else if (h < 240.0f)
            {
                r = minVal;
                g = -(h - 240.0f) * dif / 60.0f + minVal;
                b = maxVal;
            }
            else if (h < 300.0f)
            {
                r = (h - 240.0f) * dif / 60.0f + minVal;
                g = minVal;
                b = maxVal;
            }
            else if (h <= 360.0f)
            {
                r = maxVal;
                g = minVal;
                b = -(h - 360.0f) * dif / 60.0f + minVal;
            }
            else
            {
                r = 0;
                g = 0;
                b = 0;
            }
        }

        col[0] = max(0.0f, min(255.0f, r * 255.0f));
        col[1] = max(0.0f, min(255.0f, g * 255.0f));
        col[2] = max(0.0f, min(255.0f, b * 255.0f));
    }