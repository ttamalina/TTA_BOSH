

#include <cmath>
#include <raylib.h>

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

    ~matrix()
    {
        for(int i=0; i<h; i++)
            delete[] body[i];
        delete[] body;
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
        for(int i=0;i<h;i++)
        {
            for(int j=0;j<w;j++)
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
    void pooling_avg(int level)
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
            setSize(old_w,old_h);
        }
    }
    void pooling_max(int level)
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

                if(as_float_kernel)
                {
                    float acum = 0.0f;
                    for(int n=0; n<kernel->h; n++)
                    {
                        for(int m=0; m<kernel->h; m++)
                        {
                            const int ii = i - dy + n;
                            const int jj = j - dx + m;
                            if((ii>=0)&&(ii<h)&&(jj>=0)&&(jj<w))
                            {
                                acum += (float)body[ii][jj] *(float)((float)kernel->body[n][m]/(float)255.0f);
                            }
                        }
                    }
                    c_var = (int)(255.0*acum)/div;
                }
                else
                {
                    for(int n=0; n<kernel->h; n++)
                    {
                        for(int m=0; m<kernel->h; m++)
                        {
                            const int ii = i - dy + n;
                            const int jj = j - dx + m;
                            if((ii>=0)&&(ii<h)&&(jj>=0)&&(jj<w))
                            {
                                c_var += body[ii][jj] * kernel->body[n][m];
                            }
                        }
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
                body[i][j] = 255.0*(float)(body[i][j])/(float)max_val;
            //body[i][j] = 255*(body[i][j]-min_val)/div;
        }
    }

    void threshold(int limit)
    {
        for(int i=0; i<h; i++)
        {
            for(int j=0; j<w; j++)
            {
                if(body[i][j]<limit)
                    body[i][j]=0;
                else
                    body[i][j]=255;
            }
        }
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
                const int index =3*(i*w+j)
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
                Color c;
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

    void setColorBuffer(Color * pixel_buff)
    {
        for(int i=0;i<h;i++)
        {
            for(int j=0;j<w;j++)
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


};



