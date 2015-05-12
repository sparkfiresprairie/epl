#if !(_QuadTree_h)
#define _QuadTree_h 1
#include <functional>
#include <iostream>

/*
 * I use "region" to refer to a TreeNode.  Usually, I'm referring to a leaf
 * Hopefully, the meaning is clear from context
 *
 * On insert and remove events the tree changes size.  Some objects that
 * are in the tree may see the size of their enclosing region change.
 * During simulation, it is important to know of these changes.  So,
 * each object has a "callback" (a Procedure0 object) that is inserted with
 * it into the tree.  When the object's region is resized, the callback is
 * called.  Note, the callback should be invoked only after the insert
 * or remove operation is completed (that is, QuadTree invokes the
 * callback, not TreeNode).  This ensures that the tree is at a stable
 * state before the callback is invoked.
 *
 * === IMPORTANT note on resize callbacks ===
 * It is difficult to see at first, but...
 * during an insert event, at most one existing object will have its 
 * region resized.  That is, if we are inserting object 'x', then there
 * is at most one other object (object y) that will have its region
 * resized as a result of x's insertion.  This is because an object is
 * only inserted into a leaf.  During insertion, we decend the tree until
 * we find a leaf.  We resize that leaf if it contains an object.  Although
 * we may resize that leaf many times, it's always the same two objects that
 * are involved (the new one).
 *
 * Similarly, if we are removing object x, then there is at most one
 * other object (object y) that will have its region resized as
 * a result of y's insertion.  This is because the only way a region
 * is resized is when a region has exactly one sibling region that contains
 * an object.  When the sibling region has its object removed, then
 * all four siblings are collapsed into their parent.  If three siblings
 * had objects, and one was removed, the region would not be resized.
 */

/*
 * EE380L NOTE:
 * there's a bug minor inaccuracy in the simulation that could be fixed 
 * by using the quad tree in a slightly different way
 *
 * the defect is that we can have a "false encounter" in the following
 * circumstance
 *         -------------
 *         |     |     |
 *         |  y->|     |
 *         |-----+-----|
 *         |     |x    |
 *         |     |     |
 *         -------------
 *
 * where x and y are objects and y is moving to the right.
 * y does not enter x's region, so x does not experience a resize event.
 * y may be arbitrarily close to x, close enough to have an encounter.
 * however, the position that is used will be x's old position.
 * note that if y moved into x's region, the region resize callback
 * can update x's position variable.  That is, in *most* cases,
 * false encounters do not occur.  Only an objects most up-to-date position
 * is used when determining an encounter.
 *
 * anyway, this defect can be fixed by invoking the resize callback (or
 * some similar mechanism) on all objects that are inside regions that
 * intersect the unit circle about y.
 * that is, take the encounter distance (currently 1.0 units), and put
 * a circle around y *after* it has moved.  Any region that intersects
 * that circle may contain an object that has moved into or out of the
 * circle since its position variable was last updated.  So, any object
 * in those regions should have its position updated
 *
 * note that such an object doesn't need a new border_cross_event defined
 * (its region hasn't changed size).  We just need up-to-date position info
 * before we test for an encounter.  We know that 'y' cannot encounter
 * any objects in any other regions, so we should only update those
 * object in regions sufficiently close by
 *
 *
 * CODING NOTE: there should be more 'const' member functions on the QuadTree
 *
 */


#include <cassert>
#include <utility>
#include <vector>
#include "Point.h"

template <class Obj> class TreeNode; // used for implementation of the QuadTree

template <class Obj> 
/* NOTE class Obj must implement 
   Point position(void) const;
   This function will return the current position of the object
   */
class QuadTree {
  TreeNode<Obj>* root;
  Point uleft, lright;          // not really needed, as "root" duplicates
                                // this data, but having the copies of the 
                                // boundary points is convenient

  /* COPYING is NOT YET DEFINED NOR PERMITTED */
  QuadTree(const QuadTree<Obj>&) { assert(0); }
  QuadTree<Obj>& operator=(const QuadTree<Obj>&) {
    assert(0);
    return *this;
  }
public:
                                // insert a *reference* to the object into the 
                                // tree.  It is an error to insert an object
                                // which 'is_out_of_bounds'.
  void insert(const Obj&, const Point& pos, std::function<void(void)> = [](){});

  Obj remove(const Point&);
                                // find the identical object 'x' in the tree
                                // and remove it.  It is an error to attempt
                                // to remove an object that is not in the tree

  Obj closest(const Point&) const;    // find the (cartesian distance) closest Obj 
                                // to the specified point.  The QuadTree must
                                // not be empty (i.e., there must be a closest
                                // object)

  std::vector<Obj> nearby(const Point& center, double radius) const; 
                                // return a vector of Objs that are within 
                                // the specified circle
                                // the object located at the center of the
                                // circle is not included in the list
                                // (objects are not "nearby" to themselves)

  bool is_out_of_bounds(const Point&) const; // return true iff the Point is outside 
                                // the boundaries of this QuadTree

  double distance_to_edge(const Point& p, double rads) const; // return the distance
                                // between 'p' and the next edge to be crossed
                                // if one continues to travel in direction
                                // 'rads' (in radians).

  bool is_occupied(const Point&) const; // return true if the position is
                                // already occupied by some other object

  void update_position(const Point&, const Point&) ;
  // updates position of object to new position
   

  QuadTree(double xmin, double ymin, double xmax, double ymax) {
    uleft = Point(xmin,ymax);
    lright = Point(xmax,ymin);
    root = new TreeNode<Obj>(uleft, lright); 
  }

  ~QuadTree(void);
};

template <class Obj> 
class TreeNode {
  typedef std::pair<bool, std::function<void(void)>> Result;

  Obj obj;                      // the object that is in this region
                                // (valid only if num_objects == 1)

  Point obj_pos;                // the location  of the object
                                // (valid only if num_objects == 1)

  std::function<void(void)> resize_event;      // a callback that should be invoked when
                                // this region is either merged or split
  
  typedef TreeNode<Obj>* TNPtr;
  TNPtr *child;                 // an array of four pointers to children;
                                // we maintain the invariant that child is 
                                // always NULL unless 
                                //  a) there exists two or more children
                                //     that are non-empty OR
                                //  b) one child has decendents with two
                                //     or more objects.

  unsigned num_objects;         // the number of objects inside this region
                                // (including objects inside my children)

  void split(void) {
    child = new TNPtr[4];
    double x = right() - left();
    double y = top() - bottom();
    double halfx = x / 2.0;
    double halfy = y / 2.0;

    /* 1st quadrant (the upper right quad) */
    child[0] = new TreeNode<Obj>(uleft() + Point(halfx, 0), 
                                 lright() + Point(0, halfy));

    /* 2nd quadrant (upper left quad) */
    child[1] = new TreeNode<Obj>(uleft(), uleft() + Point(halfx, -halfy));

    /* 3rd quadrant (lower left quad) */
    child[2] = new TreeNode<Obj>(uleft() + Point(0, -halfy),
                                 lright() + Point(-halfx, 0));

    /* 3th quadrant (lower right quad) */
    child[3] = new TreeNode<Obj>(uleft() + Point(halfx, -halfy), lright());

    unsigned k;                 // checked at end of "for" loop
    std::function<void(void)> dummy = [](){};           // not used
    for (k = 0; k < 4; k++) {
      if (child[k]->in_bounds(obj_pos)) {
        bool tmp = child[k]->insert(obj, obj_pos, resize_event, dummy);
        assert(tmp);
        break;
      }
    }
    assert(k < 4);
  }

  void merge() {
    assert(num_objects == 1);       // must have exactly one obj

    /* take the object from our child */
    for (unsigned k = 0; k < 4; k++) {
      if (!child[k]->is_empty()) {
        obj = child[k]->obj;
        obj_pos = child[k]->obj_pos;
        resize_event = child[k]->resize_event;
      }
      delete child[k];
    }

    delete[] child;
    child = 0;
  }
    
  /* 
   * does a circle centered about 'center' with radius 'dist'
   * intersect any part of the current region?
   *
   * Technique: find the point on the boundary of this region and
   * measure the distance between that point and 'center'
   *
   * NOTE: this code assumes all four boundary edges are inside
   *   the region (usually we assume only the top and left edges
   *   are inside).  
   */
  bool intersects(const Point& center, double dist) const {
    if (in_bounds(center)) return true;

    double xval, yval;          // x and y coords of point on boundary
                                // nearest center
                                // NOTE: the following code assumes that
                                // 'center' is not inside this region
    
    xval = center.xpos; yval = center.ypos;

    if (xval < left()) xval = left();
    if (xval > right()) xval = right();
    if (yval < bottom()) yval = bottom();
    if (yval > top()) yval = top();
    
    Point edge_pt(xval, yval);  // this is the point on the edge closest to
                                // 'center'

    return (center.distance(edge_pt) <= dist);
  }


  TreeNode(const TreeNode<Obj>&) { assert(0); }
  TreeNode<Obj>& operator=(const TreeNode<Obj>&) {
    assert(0);
    return *this;
  }

  /* return which of the four child quadrants is closest to 'center' */
  unsigned nearest_region(const Point& center) const {

    /* assume quadrant 1 is closest to the object */
    unsigned region = 0; 
    double best_dist = center.distance(uright());
    double d;
    
    /* check if quadrant 2 is closer */
    d = center.distance(uleft());
    if (d < best_dist) {
      best_dist = d;
      region = 1;
    }

    /* check if quadrant 3 is closer */
    d = center.distance(lleft());
    if (d < best_dist) {
      best_dist = d;
      region = 2;
    }
    
    /* check if quadrant 4 is closer */
    d = center.distance(lright());
    if (d < best_dist) {
      best_dist = d;
      region = 3;
    }

    return region;
  }


  Point _uleft;                  // upper left corner
  Point _lright;                 // lower right corner
public:

  const Point& uleft(void) const { return _uleft; }
  const Point& lright(void) const { return _lright; }
  Point lleft(void) const {  return Point(uleft().xpos, lright().ypos); }
  Point uright(void) const { return Point(lright().xpos, uleft().ypos); }
  double left(void) const { return uleft().xpos; }
  double right(void) const { return lright().xpos; }
  double top(void) const { return uleft().ypos; }
  double bottom(void) const { return lright().ypos; }

  std::function<void(void)> get_callbk(void) const { return resize_event;}

  TreeNode(const Point& _uleft, const Point& _lright) {
    this->_uleft = _uleft; this->_lright = _lright; 
    child = (TNPtr*) 0;
    num_objects = 0;
  }

  ~TreeNode(void) {
    if (child) {
      for (unsigned k = 0; k < 4; k++)
        delete child[k];
      delete[] child;
    }
  }

  bool is_leaf(void) const { return child == (const TNPtr*) 0; }

  bool is_empty(void) const { return (num_objects == 0) && is_leaf(); }

  bool in_bounds(const Point& p) const {
    return p.xpos >= left() &&
      p.ypos <= top() &&
      p.xpos < right() &&
      p.ypos > bottom();
  }

  /* new_resize is the callback for newobj
     invoke_this is an output parameter.  It is the resize callback for
     the object who's region gets resized */
  bool insert(const Obj& newobj, const Point& pos, std::function<void(void)> new_resize,
                std::function<void(void)>& invoke_this) {
    if (! in_bounds(pos)) return false;

    if (is_empty()) {
      obj = newobj;
      obj_pos = pos;
      resize_event = new_resize;
      num_objects += 1;
      return true;
    }
    else {
      if (child == (const TNPtr*) 0) {
        invoke_this = resize_event;
        split();
      }
      unsigned k;               // checked at end of for loop
      for (k = 0; k < 4; k++) 
        if (child[k]->insert(newobj, pos, new_resize, invoke_this)) break;
      assert(k < 4);
      num_objects += 1;
      return true;
    }

    /* NOT REACHED */
  }

  bool remove(const Point& pos, Obj& oldobj, std::function<void(void)>& invoke_this) {
    if (!in_bounds(pos)) return false;
    assert(num_objects > 0);

    /* first, simply remove the object, and keep 'num_objects' correct */
    if (num_objects == 1) {
      if (pos != obj_pos) {
      	std::cout << "oh shit\n";
      }
      assert(pos == obj_pos);
      oldobj = obj;
      resize_event = std::function<void(void)>();
      num_objects -= 1;
    }
    else {
      assert(child != (TNPtr*) 0);
      unsigned k;               // checked at end of "for" loop
      for (k = 0; k < 4; k++) {
        if (child[k]->in_bounds(pos)) {
          bool tmp = child[k]->remove(pos, oldobj, invoke_this);
          assert(tmp);
          break;
        }
      }
      assert(k < 4);
      num_objects -= 1;
    }

    /* second, clean up so that our invariants are maintained */
    if (num_objects == 1) {
      merge();
      invoke_this = resize_event;
    }

    return true;
  }


  /*
   * return the vector of objects (not including one at 'center') that
   * are inside this region, and also not more than 'dist' units
   * away from 'center'
   */
  void find_nearby(std::vector<Obj>& list, const Point& center, 
                   double dist) const {
    if (is_empty()) return;
    if (! intersects(center, dist)) return;

    if (num_objects == 1) {
      if (obj_pos != center && center.distance(obj_pos) <= dist)
        list.push_back(obj);
    }
    else {
      for (unsigned k = 0; k < 4; k++) {
        child[k]->find_nearby(list, center, dist);
      }
    }
  }

  /*
   * return the closest object to 'center' that is within this region
   * (other than 'center' itself).  Consider only objects that are
   * at most dist' units away
   *
   * if no object can be found, return a pair with 'first' == false
   *
   * Technique: search the sub regions in order to minimize search time
   *   Do this by searching first inside the nearest region to 
   *   'center.position()'
   */
  std::pair<bool,Obj> closest(const Point& center, double& dist) const {
    /* three cases, 0 objects, 1 object or more than one object
       are in this region */
    if (! intersects(center, dist)) 
      return std::pair<bool,Obj>(false, Obj());

    if (num_objects == 0) 
      return std::pair<bool,Obj>(false, Obj());

    else if (num_objects == 1) {
      double d = center.distance(obj_pos);
      if (d < dist && obj_pos != center) {
        dist = d;
        return std::pair<bool,Obj>(true, obj);
      }
      else return std::pair<bool,Obj>(false, Obj());
    }

    else if (num_objects > 1) { // "else if" is for documentation purposes
                                // (must be the case that num_object > 1)
      std::pair<bool, Obj> result(false, Obj());

      unsigned first_region = nearest_region(center);

      for (unsigned k = 0; k < 4; k++) {
        unsigned region = (k + first_region) % 4;
        std::pair<bool,Obj> tmp = child[region]->closest(center, dist);
        if (tmp.first) result = tmp;
      }
      return result;
    }
    /* NOT REACHED */
    return std::pair<bool,Obj>();
  }

  /* return the leaf node where this object would be (or is) in the tree */
  /* For the meantime replace Nil<TreeNode<Obj> > with (TNPtr) 0  */

  std::pair< TreeNode<Obj>*, TreeNode<Obj>*>
  find_leaf(const Point& pos, const TreeNode<Obj>* parent= (TNPtr)0 )  const
  {
    assert(in_bounds(pos));
    if (is_leaf()) return std::make_pair( (TNPtr)this, (TNPtr)parent);
    else {
      for (unsigned k = 0; k < 4; k++) {
        if (child[k]->in_bounds(pos))
          return child[k]->find_leaf(pos, this);
      }
      /* NOT REACHED */
      assert(0);
      return std::make_pair( (TNPtr) 0,  (TNPtr) 0);
    }
    /* NOT REACHED */
  }

  bool is_occupied(const Point& x) const {
    const TreeNode<Obj>* leaf = find_leaf(x).first;
    if (leaf->is_empty()) return false;
    else 
      return leaf->obj_pos == x;
  }

  unsigned check_tree(void) const {
    if (is_leaf()) {
      assert(num_objects < 2);
      return num_objects;
    }
    else {
      unsigned child_nums = 0;
      for (int k = 0; k < 4; ++k) 
        child_nums += child[k]->check_tree();
      assert(num_objects == child_nums && child_nums > 1);
      return child_nums;
    }
  }

  friend class QuadTree<Obj>;
};


template <class Obj>
QuadTree<Obj>::~QuadTree(void) {
  delete root;
}

template <class Obj>
void QuadTree<Obj>::insert(const Obj& obj, const Point& pos, 
                           std::function<void(void)> resize) {
  std::function<void(void)> callback = [](){};
  bool is_ok = root->insert(obj, pos, resize, callback);
  assert(is_ok);
  callback();
}
         
template <class Obj>
Obj QuadTree<Obj>::remove(const Point& pos) {
  std::function<void(void)> callback = [](){};
  Obj result;
  bool is_ok = root->remove(pos, result, callback);
  assert(is_ok);
  callback();
  return result;
}

template <class Obj>
Obj QuadTree<Obj>::closest(const Point& pos) const {
  double dist = HUGE;
  std::pair<bool,Obj> tmp = root->closest(pos, dist);
  assert(tmp.first);
  return tmp.second;
}

template <class Obj>
std::vector<Obj> QuadTree<Obj>::nearby(const Point& pos, double dist) const {
  std::vector<Obj> result;
  root->find_nearby(result, pos, dist);
  return result;
}

template <class Obj>
bool QuadTree<Obj>::is_out_of_bounds(const Point& pos) const {
  return ! root->in_bounds(pos);
}

template <class Obj>
double QuadTree<Obj>::distance_to_edge(const Point& pos, double course) const {
  assert(root != (TreeNode<Obj>*) 0);
  const TreeNode<Obj>* leaf = root->find_leaf(pos).first;
  assert(leaf != (TreeNode<Obj>*) 0);
  
  double cos_theta = cos(course);
  double sin_theta = sin(course);

  double xdist = 0.0;         // distance to nearest vertical boundary
  double ydist = 0.0;         // distance to nearest horizontal boundary

  if (cos_theta < 0.0)        // headed left
    xdist = pos.xpos - leaf->left();
  else
    xdist = leaf->right() - pos.xpos;

  if (cos_theta < 0.0) cos_theta = - cos_theta;
  if (cos_theta > Point::tolerance) xdist = xdist / cos_theta;
  else xdist = HUGE;
  
  if (sin_theta > 0.0)        // headed up
    ydist = leaf->top() - pos.ypos;
  else
    ydist = pos.ypos - leaf->bottom();

  if (sin_theta < 0.0) sin_theta = - sin_theta;
  if (sin_theta > Point::tolerance) ydist = ydist / sin_theta;
  else ydist = HUGE;
  
  assert(xdist >= 0.0 && ydist >= 0.0);
  
  if (xdist < ydist) return xdist;
  else return ydist;
}

template <class Obj>
bool QuadTree<Obj>::is_occupied(const Point& pos) const {
  return root->is_occupied(pos);
}


template <class Obj>
void QuadTree<Obj>::update_position(const Point& pos_old, 
                                    const Point& pos_new) {
  
  std::pair<TreeNode<Obj>*, TreeNode<Obj>*> res = root->find_leaf(pos_old);
  TreeNode<Obj>* leaf = res.first;
  TreeNode<Obj>* parent = res.second;
  if (pos_old != leaf->obj_pos) {
    std::cerr << "Object Position: (" << pos_old.xpos << ", " << pos_old.ypos << ")" << std::endl;
    std::cerr << "Leaf Position: (" << leaf->obj_pos.xpos << ", " << leaf->obj_pos.ypos << ")" << std::endl;
  }
  assert(pos_old == leaf->obj_pos);

  /* three cases: */
  if (leaf->in_bounds(pos_new)) { // case 1: no callbacks
    /* for case 1 we know the object did not leave it's bounding leaf */
    leaf->obj_pos = pos_new;
  } else if (parent->in_bounds(pos_new)) { // case 2: at most one callback
    /* for case 2 we know the object left it's bounding leaf,
       but it did not leaf the bounds of the parent node.
       In this case, we know that no leaves will be deleted as a result
       of moving this object.
       NOTE: new leaves may be created if the object is moving into an 
       occupied sibling. */
    std::function<void(void)> obj_callback = leaf->get_callbk();

    /* remove the object FROM THE LEAF (not from the root) to
       avoid collapsing levels in the tree */
    Obj obj;
    std::function<void(void)> null_callback = [](){};   // must be null since removing from a leaf
    bool remove_ok = leaf->remove(pos_old, obj, null_callback);
    parent->num_objects -= 1;
    assert(remove_ok);

    /* inserting from the parent level and inserting at the root level
       should be the same */
    std::function<void(void)> insert_callback = [](){};;
    bool insert_ok = parent->insert(obj, pos_new, 
                                    obj_callback, insert_callback);
    assert(insert_ok);

    /* tree is now stable, invoke the callback from inserting */
    insert_callback();
  }
  else {                        // case 3: up to two callbacks
    std::function<void(void)> obj_callback = leaf->get_callbk();

    Obj obj;
    std::function<void(void)> remove_callback = [](){};
    bool remove_ok = root->remove(pos_old, obj, remove_callback);
    assert(remove_ok);

    std::function<void(void)> insert_callback = [](){};
    bool insert_ok = root->insert(obj, pos_new, obj_callback, insert_callback);
    assert(insert_ok);

    /* now the tree is stable, invoke both callbacks */
    remove_callback();
    insert_callback();
  }
  
#ifdef DEBUG_QUADTREE
  root->check_tree();
#endif /* DEBUG_QUADTREE */

}



#endif /* !(_QuadTree_h) */
