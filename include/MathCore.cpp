

#include <cmath>
#include <raylib.h>
#include <time.h>
#define RAD(A)  (M_PI*((double)(A))/180.0)
typedef unsigned char uint8_t;

class Clock
{
private:
    float current_time;

    float CPS()
    {
        return (float)clock()/(float)CLOCKS_PER_SEC;
    }

public:
    Clock()
    {
        current_time = CPS();
    }
    bool isElapsed(float seconds)
    {
        float dif_time = CPS();
        dif_time -=current_time;
        if(dif_time>seconds)
            return true;
        else
            return false;
    }

    float getElapsed()
    {
        return CPS()-current_time;
    }

    void update()
    {
        current_time = CPS();
    }
};


struct conv_kernel
{
    int w;
    int h;
    int div;
    int **body;
    conv_kernel(int **src,int w,int h)
    {
        this->w = w;
        this->h = h;
        div=0;
        body = src;
        for( int i=0; i<h; i++)
        {
            for(int j=0; j<w; j++)
                div+=body[i][j];
        }
    }
    int operator*(conv_kernel &kernel)
    {
        int width = (w<kernel.w)?w:kernel.w;
        int height = (h<kernel.h)?h:kernel.h;
        int result = 0;
        for(int i=0; i<height; i++)
        {
            for(int j=0; j<width; j++)
            {
                result+=body[i][j]*kernel.body[i][j];
            }
        }
        return result/kernel.div;
    }
};

struct matrix
{
    int **body;
    int w;
    int h;
    matrix(int w,int h)
    {
        this->w = w;
        this->h = h;
        body = new int*[h];
        for(int i=0; i<h; i++)
        {
            body[i] = new int[w]();
        }
    }

    matrix(const matrix &m):matrix(m.w,m.h)
    {
        for(int i=0; i<h; i++)
        {
            for(int j=0; j<w; j++)
            {
                body[i][j]=m.body[i][j];
            }
        }
    }

    matrix(const uint8_t * pixel_buffer,int chanel,int w,int h):matrix(w,h)
    {
        int k=0;
        for(int i=0; i<h; i++)
        {
            for(int j=0; j<w; j++)
            {
                body[i][j] = pixel_buffer[k+chanel];
                k+=3;
            }
        }
    }

    ~matrix()
    {
        for(int i=0; i<h; i++)
            delete[] body[i];
        delete[] body;
    }
    void setPixelBuffer(uint8_t *pixel_buff)
    {
        int k = 0 ;
        for(int i=0; i<h; i++)
        {
            for(int j=0; j<w; j++)
            {
                pixel_buff[k] = body[i][j];
                pixel_buff[k+1] = body[i][j];
                pixel_buff[k+2] = body[i][j];
                k+=3;
            }
        }
    }

    matrix * subMat(int start_x,int start_y,int end_x, int end_y)
    {
        if((start_x>=0)&&(start_y>=0)&&(end_x<=w)&&(end_y<=h))
        {
            int n_h = end_y - start_y,n_w = end_x - start_x;

            if((n_h>0)&&(n_w>0))
            {
                matrix * m = new matrix(n_w,n_h);

                for(int i = start_y; i<end_y; i++)
                    for(int j= start_x; j<end_x; j++)
                        m->body[i-start_y][j-start_x] = body[i][j];
                return m;
            }
        }
        return nullptr;
    }

    void setSize(int w,int h)
    {
        if((w>0)&&(h>0)&&((w!=this->w)||(h!=this->h)))
        {
            int **new_mat = new int*[h];

            for(int i=0; i<h; i++)
                new_mat[i] = new int[w]();

            int old_w = this->w, old_h = this->h;

            float step_x=(float)old_w/(float)w;
            float step_y=(float)old_h/(float)h,x=0.0f,y=0.0f;
            for(int i=0; i<h; i++)
            {
                x=0.0f;
                for(int j=0; j<w; j++)
                {
                    new_mat[i][j]=body[(int)trunc(y)][(int)trunc(x)];
                    x+=step_x;
                }
                y+=step_y;
            }
            for(int i=0; i<old_h; i++)
                delete[] body[i];
            delete[] body;
            body = new_mat;
            this->w = w;
            this->h = h;
        }
    }

    void scale(float ration)
    {
        setSize(w*ration,h*ration);
    }

    matrix clone(matrix &m)
    {
        matrix local(m);
        return local;
    }

    Rectangle getBorder()
    {
        Vector4 v= {0};
        for(int j=0; j<w; j++)
            for(int i=0; i<h; i++)
                if(body[i][j])
                {
                    v.x=j;
                    break;
                }

        for(int i=0; i<h; i++)
            for(int j=0; j<w; j++)
            {
                if(body[i][j])
                {
                    v.y=i;
                    break;
                }
            }

        for(int j=w-1; j>=0; j--)
            for(int i=h-1; i>=0; i--)
                if(body[i][j])
                {
                    v.w=j;
                    break;
                }
        for(int i=h-1; i>=0; i--)
            for(int j=w-1; j>=0; j--)
                if(body[i][j])
                {
                    v.z=i;
                    break;
                }
        Rectangle rect;
        rect.x = v.x;
        rect.y = v.y;
        rect.width = v.w - v.x;
        rect.height = v.z - v.y;
        return rect;
    }

    void diff(matrix &m,float ration)
    {
        if((w==m.w)&&(h==m.h))
        {
            float div =0.0;
            for(int i=0; i<h; i++)
            {
                for(int j=0; j<w; j++)
                {
                    div = 100.0f*(float)m.body[i][j]/(float)body[i][j];
                    if(div>ration)
                    {
                        body[i][j]=0;
                    }
                    else
                        body[i][j]=255;
                }
            }
        }
    }

    void setColorBuffer(Color * pixel_buffer)
    {
        for(int i=0; i<h; i++)
        {
            for(int j=0; j<w; j++)
            {
                const int index = i*w+j;
                pixel_buffer[index].r = body[i][j];
                pixel_buffer[index].g = body[i][j];
                pixel_buffer[index].b = body[i][j];
                pixel_buffer[index].a = 0xFF;
            }
        }
    }

    int checkPosition(int x,int y)
    {
        if((x>=w)||(y>=h))
            return 0;
        else
            return 1;
    }
    void pooling_avg(int level,bool upScale=true)
    {
        if((level<w)&&(level<h)&&(level>1))
        {
            int old_w = w, old_h = h;
            setSize(w-w%level,h-h%level);
            int sum,i,j;
            for(i=0; i<(h-h%level); i+=level)
            {
                for(j=0; j<(w-w%level); j+=level)
                {
                    sum=0;
                    for(int k=i; k<(i+level); k++)
                        for(int m=j; m<(j+level); m++)
                            sum+=body[k][m];
                    sum=sum/(level*level);
                    for(int k=i; k<(i+level); k++)
                        for(int m=j; m<(j+level); m++)
                            body[k][m]= sum;
                }
            }
            if(upScale)
                setSize(old_w,old_h);
        }
    }
    void pooling_max(int level,bool upScale=true)
    {
        if((level<w)&&(level<h)&&(level>1))
        {
            int old_w = w, old_h = h;
            setSize(w-w%level,h-h%level);
            int maximum=0,i,j;
            for(i=0; i<(h-h%level); i+=level)
            {
                for(j=0; j<(w-w%level); j+=level)
                {
                    maximum =0;
                    for(int k=i; k<(i+level); k++)
                        for(int m=j; m<(j+level); m++)
                            if(maximum<body[k][m])
                                maximum=body[k][m];
                    for(int k=i; k<(i+level); k++)
                        for(int m=j; m<(j+level); m++)
                            body[k][m]= maximum;
                }
            }
            if(upScale)
                setSize(old_w,old_h);
        }
    }


    unsigned int sumPixels()
    {
        unsigned int sum = 0;
        for(int i=0; i<h; i++)
            for(int j=0; j<w; j++)
                sum+=body[i][j];
        return sum;
    }

    int min()
    {
        int min_val = body[0][0];

        for(int i=0; i<h; i++)
        {
            for(int j=0; j<w; j++)
                if(min_val >body[i][j])
                    min_val = body[i][j];
        }

        return min_val;
    }

    int max()
    {
        int max_val = body[0][0];

        for(int i=0; i<h; i++)
        {
            for(int j=0; j<w; j++)
                if(max_val < body[i][j])
                    max_val = body[i][j];
        }

        return max_val;
    }


    matrix* conv(matrix *kernel,bool as_float_kernel, bool padding = true)
    {
        if(kernel->h!=kernel->w)
        {
            return nullptr;
        }
        if((kernel->h%2==0))
        {
            return nullptr;
        }
        int dx = (padding)?kernel->h/2:0;
        int dy = (padding)?kernel->h/2:0;
        int new_w = (padding)?w:w-kernel->h;
        int new_h = (padding)?h:h-kernel->h;
        int div = kernel->sumPixels();
        if(div==0)
            div=1;

        matrix * local = new matrix(new_w,new_h);

        for(int i=0; i<new_h; i++)
        {
            for(int j=0; j<new_w; j++)
            {
                int c_var = 0;
                int ii = i - dy;

                if(as_float_kernel)
                {
                    float acum = 0.0f;
                    for(int n=0; n<kernel->h; n++)
                    {
                        const int * kernel_body_n = kernel->body[n];
                        int jj = j - dx;

                        for(int m=0; m<kernel->h; m++)
                        {
                            const int kernel_body_nm = kernel_body_n[m];
                            if(kernel_body_nm)
                            {
                                if((ii>=0)&&(ii<h)&&(jj>=0)&&(jj<w))
                                {
                                    if(body[ii][jj])
                                        acum += (float)body[ii][jj] *(float)((float)kernel_body_nm/(float)255.0f);
                                }
                            }
                            jj++;
                        }
                        ii++;
                    }
                    c_var = (int)(255.0*acum)/div;
                }
                else
                {
                    for(int n=0; n<kernel->h; n++)
                    {
                        int jj = j - dx;
                        const int * kernel_body_n = kernel->body[n];
                        for(int m=0; m<kernel->h; m++)
                        {
                            const int kernel_body_nm = kernel_body_n[m];
                            if(kernel_body_nm)
                            {
                                if((ii>=0)&&(ii<h)&&(jj>=0)&&(jj<w))
                                {
                                    if(body[ii][jj])
                                        c_var += body[ii][jj] * kernel_body_nm;
                                }
                            }
                            jj++;
                        }
                        ii++;
                    }
                    c_var/=div;
                }

                local->body[i][j] = c_var;

            }
        }
        return local;

    }

    void normalize()
    {
        int min_val = this->min();
        int max_val = this->max();
        int div = max_val - min_val;
        for(int i=0; i<h; i++)
        {
            for(int j=0; j<w; j++)
                //body[i][j] = 255.0*(float)(body[i][j])/(float)max_val;
                body[i][j] = 255*(body[i][j]-min_val)/div;
        }
    }

    void threshold(int limit,bool binary=true)
    {
        for(int i=0; i<h; i++)
        {
            for(int j=0; j<w; j++)
            {
                if(body[i][j]<limit)
                    body[i][j]=0;
                else if(binary)
                    body[i][j]=255;
            }
        }
    }

    matrix * hough_transform()
    {

        int rho, theta, y, x, W = w, H = h;
        int th = sqrt(W*W + H*H)/2.0;
        int tw = 360;
        matrix * h = new matrix(tw,th);

        for(rho = 0; rho < th; rho++)
        {
            for(theta = 0; theta < tw; theta++)
            {
                double C = cos(RAD(theta));
                double S = sin(RAD(theta));
                int totalPixels =0 ;
                int totalGreyColor =0;
                if ( theta < 45 || (theta > 135 && theta < 225) || theta > 315)
                {
                    for(y = 0; y < H; y++)
                    {
                        double dx = W/2.0 + (rho - (H/2.0-y)*S)/C;
                        if ( dx < 0 || dx >= W )
                            continue;
                        x = floor(dx+.5);
                        if (x == W)
                            continue;

                        totalPixels++;
                        totalGreyColor+=body[y][x];
                    }
                }
                else
                {
                    for(x = 0; x < W; x++)
                    {
                        double dy = H/2.0 - (rho - (x - W/2.0)*C)/S;
                        if ( dy < 0 || dy >= H )
                            continue;
                        y = floor(dy+.5);
                        if (y == H)
                            continue;
                        totalPixels++;
                        totalGreyColor+=body[y][x];
                    }
                }
                if(totalPixels>0)
                {
                    double dp = totalPixels;
                    h->body[rho][theta] = (int) (totalGreyColor/dp) &0xFF;
                }
            }
        }

        return h;
    }

    matrix * hough_filter(int limit)
    {
        matrix * h = hough_transform();
        matrix * m = new matrix(this->w,this->h);

        for(int rho = 0 ; rho<h->h; rho++)
        {
            for(int th = 0; th<h->w; th++)
            {
                if((h->body[rho][th]>100)&&(h->body[rho][th]<250))
                    for(int y=0; y<m->h; y++)
                    {

                        const double C=cos(RAD(th));
                        const double S=sin(RAD(th));
                        const int W = m->w;
                        const int H = m->h;
                        double dx = W/2.0 + (rho - (H/2.0-y)*S)/C;
                        if ( dx < 0 || dx >= W )
                            continue;
                        int x = floor(dx+.5);
                        if (x == W)
                            continue;
                        m->body[y][x] =255;
                    }
            }
        }

        delete h;
        return m;
    }

    matrix * sobol_filterX()
    {
        matrix * kernel_mat = new matrix(3,3);
        int **kernel = kernel_mat->body;
        kernel[0][0] = -1;
        kernel[0][1] =  0;
        kernel[0][2] =  1;
        kernel[1][0] = -2;
        kernel[1][1] =  0;
        kernel[1][2] =  2;
        kernel[2][0] = -1;
        kernel[2][1] =  0;
        kernel[2][2] =  1;

        matrix * local = conv(kernel_mat,false);

        delete kernel_mat;

        return local;
    }

    matrix * sobol_filterY()
    {
        matrix * kernel_mat = new matrix(3,3);
        int **kernel = kernel_mat->body;
        kernel[0][0] = -1;
        kernel[0][1] = -2;
        kernel[0][2] = -1;
        kernel[1][0] =  0;
        kernel[1][1] =  0;
        kernel[1][2] =  0;
        kernel[2][0] =  1;
        kernel[2][1] =  2;
        kernel[2][2] =  1;

        matrix * local = conv(kernel_mat,false);

        delete kernel_mat;

        return local;
    }

    matrix * sobol_filter()
    {
        matrix * gx = sobol_filterX();
        matrix * gy = sobol_filterX();
        matrix * g = new matrix(gx->w,gx->h);


        for(int i=0; i<gx->h; i++)
        {
            for(int j=0; j<gx->w; j++)
            {
                const int gx_val = gx->body[i][j];
                const int gy_val = gy->body[i][j];
                g->body[i][j] = sqrt((double)(gx_val*gx_val+gy_val*gy_val));
            }
        }


        delete gx;
        delete gy;
        return g;
    }

    matrix * gaussian_filter(float sigma)
    {
        const int n = 2 * (int)(2 * sigma) + 3;
        const float mean = (float)floor(n / 2.0);
        matrix * kernel = new matrix(n,n);

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
            {
                kernel->body[i][j] =(int)(255.0 * ( exp(-0.5 * (pow((i - mean) / sigma, 2.0) +
                                                        pow((j - mean) / sigma, 2.0)))
                                                    / (2 * M_PI * sigma * sigma)));
            }

        matrix * local = conv(kernel,true);

        delete kernel;

        return local;
    }


};

#define BGR_BUFFER 1
#define RGB_BUFFER 0

class frame
{
private:
    int w;
    int h;
    matrix** rgb;
public:
    frame(int w,int h)
    {
        rgb = new matrix*[3];
        this->w=w;
        this->h=h;
        for(int i=0; i<3; i++)
            rgb[i]= new matrix(w,h);
    }
    frame(frame &f)
    {
        w = f.getWidth();
        h = f.getHeight();

        rgb[0] = new matrix(*f.getRedBuffer());
        rgb[1] = new matrix(*f.getGreenBuffer());
        rgb[2] = new matrix(*f.getBlueBuffer());


    }

    frame(matrix &m)
    {
        rgb = new matrix*[3];
        w = m.w;
        h = m.h;
        for(int i=0; i<3; i++)
            rgb[i] = new matrix(m);
    }
    frame(int *BUFFER,int w,int h,int RGB_OR_BGR=0):frame(w,h)
    {
        unsigned char * pixel;
        int r_chanel,b_chanel;
        if(RGB_OR_BGR)
        {
            r_chanel = 2;
            b_chanel = 0;
        }
        else
        {
            r_chanel = 0;
            b_chanel = 2;
        }

        for(int i=0; i<h; i++)
        {
            for(int j=0; j<w; j++)
            {
                pixel=(unsigned char*)&BUFFER[i*w+j];
                rgb[0]->body[i][j]=pixel[r_chanel];
                rgb[1]->body[i][j]=pixel[1];
                rgb[2]->body[i][j]=pixel[b_chanel];
            }
        }
    }

    frame(Color *colors,int w,int h):frame(w,h)
    {
        unsigned char * pixel;
        for(int i=0; i<h; i++)
        {
            for(int j=0; j<w; j++)
            {
                pixel=(unsigned char*)&colors[i*w+j];
                rgb[0]->body[i][j]=pixel[0];
                rgb[1]->body[i][j]=pixel[1];
                rgb[2]->body[i][j]=pixel[2];
            }
        }
    }

    frame(const RGBImage & image):frame(image.width,image.height)
    {

        for(int i=0;i<h;i++)
        {
            for(int j=0;j<w;j++)
            {
                const int index =3*(i*w+j);
                rgb[0]->body[i][j]=image.data[index];
                rgb[1]->body[i][j]=image.data[index+1];
                rgb[2]->body[i][j]=image.data[index+2];
            }
        }

    }

    Color * toColorBuffer()
    {
        Color * pixel_buffer = new Color[h*w];
        for(int i =0 ; i<h; i++)
        {
            for(int j=0; j<w; j++)
            {
                const int index = i*w+j;
                pixel_buffer[index].r = rgb[0]->body[i][j];
                pixel_buffer[index].g = rgb[1]->body[i][j];
                pixel_buffer[index].b = rgb[2]->body[i][j];
                pixel_buffer[index].a = 0xFF;
            }

        }
        return pixel_buffer;
    }

    ~frame()
    {
        for(int i=0; i<3; i++)
            delete rgb[i];
        delete[] rgb;
    }

    int getWidth()
    {
        return w;
    }

    int getHeight()
    {
        return h;
    }

    const matrix* getRedBuffer()
    {
        return rgb[0];
    }
    const matrix* getGreenBuffer()
    {
        return rgb[1];
    }
    const matrix* getBlueBuffer()
    {
        return rgb[2];
    }

    frame* clone()
    {
        return new frame(*this);
    }

    matrix* toGreyScaleBuffer()
    {
        matrix *m = new matrix(w,h);
        for(int i=0; i<h; i++)
        {
            for(int j=0; j<w; j++)
            {
                m->body[i][j] = 0.4*rgb[0]->body[i][j]+
                                0.3*rgb[1]->body[i][j]+
                                0.3*rgb[2]->body[i][j];
            }
        }
        return m;
    }

    Color getPixel(int x,int y)
    {
        Color c;
        c.a=0xff;
        c.r = rgb[0]->body[y][x];
        c.g = rgb[1]->body[y][x];
        c.b = rgb[2]->body[y][x];
        return c;
    }

    void setPixel(int x,int y,Color c)
    {
        rgb[0]->body[y][x] = c.r;
        rgb[1]->body[y][x] = c.g;
        rgb[2]->body[y][x] = c.b;
    }

    matrix** getBuffers()
    {
        return rgb;
    }

    void setPixelBuffer(uint8_t *pixel_buff,bool RGB = true)
    {

        int red = (RGB)?0:2;
        int blue = (RGB)?2:0;
        int k = 0 ;
        for(int i=0; i<h; i++)
        {
            for(int j=0; j<w; j++)
            {
                pixel_buff[k] = rgb[red]->body[i][j];
                pixel_buff[k+1] = rgb[1]->body[i][j];
                pixel_buff[k+2] = rgb[blue]->body[i][j];
                k+=3;
            }
        }
    }

    void setColorBuffer(Color * pixel_buff)
    {
        for(int i=0; i<h; i++)
        {
            for(int j=0; j<w; j++)
            {
                Color c;
                c.r = rgb[0]->body[i][j];
                c.g = rgb[1]->body[i][j];
                c.b = rgb[2]->body[i][j];
                c.a = 0xFF;
                pixel_buff[i*w+j]=c;
            }
        }
    }

    void setSize(int w,int h)
    {
        this->w = w;
        this->h = h;
        for(int i=0; i<3; i++)
            rgb[i]->setSize(w,h);
    }

    void scale(float ration)
    {
        this->w *= ration;
        this->h *= ration;
        for(int i=0; i<3; i++)
            rgb[i]->setSize(this->w,this->h);
    }


};



