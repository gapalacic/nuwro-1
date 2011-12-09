#include "kaskada7.h"
#include "beam.h"
#include <TFile.h>
#include <TTree.h>
#include "args.h"
#include "nucleusmaker.h"
#include "beam_uniform.h"

int main(int argc, const char** argv)
{  
  args a("kaskada","kaskada.txt","kaskada.root");
  a.read(argc,argv);   
  params p;
  p.read(a.input);
  p.read(a.params,"Command line");
  p.list();
  frandom_init(p.random_seed);
  
  event *e=new event;
  TFile *f= new TFile(a.output,"recreate");
  TTree * t2=new TTree("treeout","Tree of events");
  t2->Branch("e","event",&e); ///< tree1 has only one branch (branch of events)

  try
  {
	  nucleus* nucl= make_nucleus(p);
	  beam_uniform b(p);
	  b.check_energy();         
  }
  catch(const char* w)
  {
	  cout<<endl<<"Exception:     "<<w<<endl<<endl;
	  return 1;
  }

  beam_uniform b(p);
  nucleus* nucl= make_nucleus(p); 

  for(int i=0;i<p.number_of_events;i++)
  {  cout<<"event="<<i<<" begin      \r"; 
     e=new event;
     particle p0=b.shoot();
     p0.r=start_point(nucl,p);
     e->out.push_back(p0);
     e->flag.qel = 0;
	 e->in.push_back(p0);
	 e->in.push_back(p0);
	 e->in[1].p4().t = 0;
     kaskadaevent(p,*e);
     t2->Fill();
     delete e;
     cout<<"event "<<i<<": completed.\r"; 

  }
  
  f->Write();
  delete nucl;
  delete t2;
  delete f;
  genrand_write_state(); 
  return 0;
}