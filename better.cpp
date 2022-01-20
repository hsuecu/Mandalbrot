#include <wx/wx.h>
#include <wx/sizer.h>
#include <bits/stdc++.h>

using namespace std;

class Mandalbrot{
	public:
		int width;
		int height;
		wxImage * image;
        wxBitmap * bmp;
		
		double xi;
		double xf;
		double yi;
		double yf;
		
		double xinc;
		double yinc;
		
		int MAX_ITR;
		double MAX_UNSTABLE;
		int CFC;
		
		Mandalbrot(int w, int h){
			width = w;
			height = h;
			xi = -1;
			xf = 1;
			yi = -1;
			yf = 1;
			xinc = (xf-xi)/height;
			yinc = (yf-yi)/width;
			MAX_ITR = 100;
			MAX_UNSTABLE = 4;
			CFC = 200;
            image = new wxImage(width, height);
		}
		
		void update(){
			double x = xi;
			double y = yi;
			
			xinc = (xf-xi)/height;
			yinc = (yf-yi)/width;
			
			int count = 0;
			double mag = 0;
			
			double zi = 0;
			double zr = 0;
			
			double rr = 0;
			double ii = 0;
			
			for(int i = 0; i < height; i++){
				for(int j = 0; j < width; j++){
					mag=0;
					zr=0, zi =0;
					while((count++ < MAX_ITR) && (mag < MAX_UNSTABLE)){
						rr = zr;
						ii = zi;
						zr = rr*rr - ii*ii;
						zi = 2*rr*ii;
						
						zr += x;
						zi += y;
						mag = pow(zr*zr + zi*zi,0.5);
					}
					int temp = 255 - (int)(count * CFC/MAX_ITR);
                    image->SetRGB(j, i, temp, temp, temp);
					x = x + xinc;
					count = 0;
				}
				x = xi;
				y = y + yinc;
			}
		}

        void thread_update(int scale){
            vector<thread> *th = new vector<thread>(0);
            double factor = (xf-xi)/width;
            int s = width/scale;
            for(int i = 0; i < scale; i++){
                th->push_back(thread(thread_update_util, this, xi + i * s * factor , factor, i * s, s));
            }
            for(auto& itr: *th) itr.join();

			delete(th);
        }

        static void thread_update_util(Mandalbrot *obj, double from, double factor, int imgx, int scale){
            //
            double x = from;
			double y = obj->yi;

			obj->yinc = (obj->yf-obj->yi)/obj->height;
			
			int count = 0;
			double mag = 0;
			
			double zi = 0;
			double zr = 0;
			
			double rr = 0;
			double ii = 0;
            //
            for(int i = 0; i < obj->height; i++){
                for(int j = imgx; j < imgx + scale; j++){
                    mag=0;
					zr=0, zi =0;
					while((count++ < obj->MAX_ITR) && (mag < obj->MAX_UNSTABLE)){
						rr = zr;
						ii = zi;
						zr = rr*rr - ii*ii;
						zi = 2*rr*ii;
						
						zr += x;
						zi += y;
						mag = pow(zr*zr + zi*zi,0.5);
					}
					int temp = 255 - (int)(count * obj->CFC/obj->MAX_ITR);
                    obj->image->SetRGB(j, i, temp, temp, temp);
					x = x + factor;
					count = 0;
                }
                x = from;
				y = y + obj->yinc;
            }
        }
		
		void up(){
			double yscalef = (yf - yi)/6;
			yi -= yscalef;
			yf -= yscalef;
		}
		void down(){
			double yscalef = (yf - yi)/6;
			yi += yscalef;
			yf += yscalef;
		}
		void left(){
			double xscalef = (xf - xi)/6;
			xi -= xscalef;
			xf -= xscalef;
		}
		void right(){
			double xscalef = (xf - xi)/6;
			xi += xscalef;
			xf += xscalef;
		}
		void zoomin(){
			double xscalef = (xf - xi)/6;
			double yscalef = (yf - yi)/6;
			xi += xscalef;
			xf -= xscalef;
			yi += yscalef;
			yf -= yscalef;
		}
		void zoomout(){
			double xscalef = (xf - xi)/6;
			double yscalef = (yf - yi)/6;
			xi -= xscalef;
			xf += xscalef;
			yi -= yscalef;
			yf += yscalef;
		}
		void refine(){
			MAX_ITR += 10;
		}
		void blur(){
			
			if(MAX_ITR > 10)MAX_ITR -= 10;
		}
		
        void detailed(){
			if(CFC < 255)CFC += 1;
        }

        void undetailed(){
			if(CFC > 0)CFC -= 1;
        }
		void p5Dump(){
            ofstream out("out.pgm");
			out << "P5" << endl;
			out << width << " " << height << endl;
			out << "255" << endl;
			for(int i = 0; i < height; i++){
				for(int j = 0; j < width; j++){
					out<<image->GetRed(j, i);
				}
			}
			out.close();
			cout <<endl<< "saved! P5 pgm" << endl;
			system("sleep 1");
		}
};


Mandalbrot * fractal;

class BasicDrawPane : public wxPanel{
    public:
        BasicDrawPane(wxFrame* parent);
        void paintEvent(wxPaintEvent & evt);
        void paintNow();
        void keyPressed(wxKeyEvent& evt);
        void render(wxDC& dc);
        DECLARE_EVENT_TABLE()
};


class MyApp: public wxApp{
    public:
        bool OnInit();
        wxFrame *frame;
        BasicDrawPane * drawPane;  
};

IMPLEMENT_APP(MyApp)


bool MyApp::OnInit(){
    wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    fractal = new Mandalbrot(800, 800);
    frame = new wxFrame((wxFrame *)NULL, -1,  wxT("Hello wxDC"), wxPoint(50,50), wxSize(900,900));
	
    drawPane = new BasicDrawPane( (wxFrame*) frame );
    sizer->Add(drawPane, 1, wxEXPAND);
	
    frame->SetSizer(sizer);
    frame->SetAutoLayout(true);
	
    frame->Show();
    return true;
} 

BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)
EVT_KEY_DOWN(BasicDrawPane::keyPressed)
EVT_PAINT(BasicDrawPane::paintEvent)

END_EVENT_TABLE()

BasicDrawPane::BasicDrawPane(wxFrame* parent) :wxPanel(parent){

}

void BasicDrawPane::paintEvent(wxPaintEvent & evt){
    wxPaintDC dc(this);
    render(dc);
}

void BasicDrawPane::keyPressed(wxKeyEvent & evt){
    char t = evt.GetUnicodeKey();
    switch(t){
        case 'Z':fractal->zoomin();break;
        case 'A':fractal->zoomout();break;
        case 'H':fractal->left();break;
        case 'J':fractal->right();break;
        case 'K':fractal->up();break;
        case 'L':fractal->down();break;
        case 'R':fractal->refine();break;
        case 'B':fractal->blur();break;
        case 'D':fractal->detailed();break;
        case 'U':fractal->undetailed();break;
        case 'S':fractal->p5Dump();break;
    }
    paintNow();
}

void BasicDrawPane::paintNow(){
    wxClientDC dc(this);
    render(dc);
}

void BasicDrawPane::render(wxDC&  dc){
    system("clear");
    cout << "x: [" << fractal->xi << "," << fractal->xf << "]" << endl;
    cout << "y: [" << fractal->yi << "," << fractal->yf << "]" << endl;
    cout << "zoom_scale: [" << 2/(fractal->xf - fractal->xi) << "]"<< endl; 
	cout << "max_itr: " << fractal->MAX_ITR << endl;
	cout << "max_unstable: " << fractal->MAX_UNSTABLE << endl;
	cout << "CFC: "<< fractal->CFC << endl;
    fractal->thread_update(10);
	fractal->bmp = new wxBitmap(*(fractal->image));
	dc.DrawBitmap(*(fractal->bmp), 30, 30);
	delete(fractal->bmp);
}