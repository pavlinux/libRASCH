#ifndef RA_VIEW_GTK_H
#define RA_VIEW_GTK_H

#include <gtkmm/paned.h>

#include "ra_view_general.h"

class ra_view_gtk : public Gtk::VPaned, public virtual ra_view_general
{
 public:
      ra_view_gtk(meas_handle h);
      ~ra_view_gtk();

      virtual void do_update();

      void get_child_dimensions();
      
 protected:
/*       virtual bool event(QEvent * ev); */
      
      void show_focus(int prev_focus = -1);
      
      virtual void *create_main_view();
      virtual int add_single_view(void *view);
      virtual int get_view_dimensions(void *vh, long *width, long *height);
      virtual int pos_view(void *vh, long x, long y);

      int curr_pane;
      int num_sub_panes;
      Gtk::VPaned **sub_panes;
}; // class ra_view_gtk


#endif // RA_VIEW_GTK_H
