#include <stdint.h>

#define CREATE_SYSV_VALIST(A) \
  va_list sysv_varargs; \
  sysv_varargs->gpr=8; \
  sysv_varargs->fpr=8; \
  sysv_varargs->overflow_arg_area=A;

#ifdef SYSV_VARARG
#define VARARGS sysv_varargs
#define PREPARE_VALIST CREATE_SYSV_VALIST(emu->scratch)
#define VARARGS_(A) sysv_varargs
#define PREPARE_VALIST_(A) CREATE_SYSV_VALIST(A)
#else
#define VARARGS emu->scratch
#define PREPARE_VALIST do {} while(0)
#define VARARGS_(A) A
#define PREPARE_VALIST_(A) do {} while(0)
#endif


void myStackAlign(const char* fmt, uint32_t* st, uint32_t* mystack);
void myStackAlignGVariantNew(const char* fmt, uint32_t* st, uint32_t* mystack);
void myStackAlignW(const char* fmt, uint32_t* st, uint32_t* mystack);

void AlignStat64(const void* source, void* dest);
void UnalignStat64(const void* source, void* dest);

void UnalignStatFS64(const void* source, void* dest);

void UnalignOggVorbis(void* dest, void* source); // Arm -> x86
void AlignOggVorbis(void* dest, void* source);   // x86 -> Arm

void UnalignVorbisDspState(void* dest, void* source); // Arm -> x86
void AlignVorbisDspState(void* dest, void* source);   // x86 -> Arm

void UnalignVorbisBlock(void* dest, void* source); // Arm -> x86
void AlignVorbisBlock(void* dest, void* source);   // x86 -> Arm

void UnalignEpollEvent(void* dest, void* source, int nbr); // Arm -> x86
void AlignEpollEvent(void* dest, void* source, int nbr); // x86 -> Arm

void UnalignSmpegInfo(void* dest, void* source); // Arm -> x86
void AlignSmpegInfo(void* dest, void* source);   // x86 -> Arm

// stat64 is packed on i386, not on arm (and possibly other structures)
#undef st_atime
#undef st_atime_nsec
#undef st_mtime
#undef st_mtime_nsec
#undef st_ctime
#undef st_ctime_nsec
struct i386_stat64 {
	uint64_t	st_dev;
	uint8_t		__pad0[4];
	uint32_t		__st_ino;
	uint32_t		st_mode;
	uint32_t		st_nlink;
	uint32_t		st_uid;
	uint32_t		st_gid;
	uint64_t	st_rdev;
	uint8_t		__pad3[4];
	int64_t		st_size;
	uint32_t		st_blksize;
	uint64_t		st_blocks;
	uint32_t	st_atime;
	uint32_t	st_atime_nsec;
	uint32_t	st_mtime;
	uint32_t	st_mtime_nsec;
	uint32_t	st_ctime;
	uint32_t	st_ctime_nsec;
	uint64_t	st_ino;
} __attribute__((packed));

struct i386_fsid {
  int     val[2];
} __attribute__((packed));

struct i386_statfs {
  uint32_t    f_type;
  uint32_t    f_bsize;
  uint32_t    f_blocks;
  uint32_t    f_bfree;
  uint32_t    f_bavail;
  uint32_t    f_files;
  uint32_t    f_ffree;
  struct i386_fsid f_fsid;
  uint32_t    f_namelen;
  uint32_t    f_frsize;
  uint32_t    f_flags;
  uint32_t    f_spare[4];
} __attribute__((packed));

struct i386_statfs64 {
  uint32_t    f_type;
  uint32_t    f_bsize;
  uint64_t    f_blocks;
  uint64_t    f_bfree;
  uint64_t    f_bavail;
  uint64_t    f_files;
  uint64_t    f_ffree;
  struct i386_fsid f_fsid;
  uint32_t    f_namelen;
  uint32_t    f_frsize;
  uint32_t    f_flags;
  uint32_t    f_spare[4];
} __attribute__((packed));

typedef struct {
  unsigned char *data;
  int storage;
  int fill;
  int returned;

  int unsynced;
  int headerbytes;
  int bodybytes;
} ogg_sync_state;

typedef struct {
  unsigned char   *body_data;    /* bytes from packet bodies */
  long    body_storage;          /* storage elements allocated */
  long    body_fill;             /* elements stored; fill mark */
  long    body_returned;         /* elements of fill returned */


  int     *lacing_vals;      /* The values that will go to the segment table */
  int64_t *granule_vals; /* granulepos values for headers. Not compact
                                this way, but it is simple coupled to the
                                lacing fifo */
  long    lacing_storage;
  long    lacing_fill;
  long    lacing_packet;
  long    lacing_returned;

  unsigned char    header[282];      /* working space for header encode */
  int              header_fill;

  int     e_o_s;          /* set when we have buffered the last packet in the
                             logical bitstream */
  int     b_o_s;          /* set after we've written the initial page
                             of a logical bitstream */
  long    serialno;
  long    pageno;
  int64_t  packetno;  /* sequence number for decode; the framing
                             knows where there's a hole in the data,
                             but we need coupling so that the codec
                             (which is in a separate abstraction
                             layer) also knows about the gap */
  int64_t   granulepos;

} ogg_stream_state;

typedef struct vorbis_dsp_state {
  int analysisp;
  void *vi; //vorbis_info

  float **pcm;
  float **pcmret;
  int      pcm_storage;
  int      pcm_current;
  int      pcm_returned;

  int  preextrapolate;
  int  eofflag;

  long lW;
  long W;
  long nW;
  long centerW;

  int64_t granulepos;
  int64_t sequence;

  int64_t glue_bits;
  int64_t time_bits;
  int64_t floor_bits;
  int64_t res_bits;

  void       *backend_state;
} vorbis_dsp_state;

typedef struct {
  long endbyte;
  int  endbit;

  unsigned char *buffer;
  unsigned char *ptr;
  long storage;
} oggpack_buffer;

typedef struct vorbis_block {
  /* necessary stream state for linking to the framing abstraction */
  float  **pcm;       /* this is a pointer into local storage */
  oggpack_buffer opb;

  long  lW;
  long  W;
  long  nW;
  int   pcmend;
  int   mode;

  int         eofflag;
  int64_t granulepos;
  int64_t sequence;
  vorbis_dsp_state *vd; /* For read-only access of configuration */

  /* local storage to avoid remallocing; it's up to the mapping to
     structure it */
  void               *localstore;
  long                localtop;
  long                localalloc;
  long                totaluse;
  struct alloc_chain *reap;

  /* bitmetrics for the frame */
  long glue_bits;
  long time_bits;
  long floor_bits;
  long res_bits;

  void *internal;

} vorbis_block;

typedef struct {
  size_t (*read_func)  (void *ptr, size_t size, size_t nmemb, void *datasource);
  int    (*seek_func)  (void *datasource, int64_t offset, int whence);
  int    (*close_func) (void *datasource);
  long   (*tell_func)  (void *datasource);
} ov_callbacks;

typedef struct OggVorbis  {
  void            *datasource; /* Pointer to a FILE *, etc. */
  int              seekable;
  int64_t      offset;
  int64_t      end;
  ogg_sync_state   oy;

  /* If the FILE handle isn't seekable (eg, a pipe), only the current
     stream appears */
  int              links;
  int64_t     *offsets;
  int64_t     *dataoffsets;
  long            *serialnos;
  int64_t     *pcmlengths; /* overloaded to maintain binary
                                  compatibility; x2 size, stores both
                                  beginning and end values */
  void     *vi; //vorbis_info
  void  *vc;    //vorbis_comment

  /* Decoding working state local storage */
  int64_t      pcm_offset;
  int              ready_state;
  long             current_serialno;
  int              current_link;

  double           bittrack;
  double           samptrack;

  ogg_stream_state os; /* take physical pages, weld into a logical
                          stream of packets */
  vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
  vorbis_block     vb; /* local working space for packet->PCM decode */

  ov_callbacks callbacks;

} OggVorbis;

typedef struct my_SMPEG_Info_s {
    int has_audio;
    int has_video;
    int width;
    int height;
    int current_frame;
    double current_fps;
    char audio_string[80];
    int  audio_current_frame;
    uint32_t current_offset;
    uint32_t total_size;
    double current_time;
    double total_time;
} my_SMPEG_Info_t;

typedef struct  __attribute__((packed)) x86_ftsent_s {
        struct x86_ftsent_s *fts_cycle;
        struct x86_ftsent_s *fts_parent;
        struct x86_ftsent_s *fts_link;
        long fts_number;
        void *fts_pointer;
        char *fts_accpath;
        char *fts_path;
        int fts_errno;
        int fts_symfd;
        uint16_t fts_pathlen;
        uint16_t fts_namelen;
        uintptr_t fts_ino;
        uint64_t fts_dev;
        uint32_t fts_nlink;
        int16_t fts_level;
        uint16_t fts_info;
        uint16_t fts_flags;
        uint16_t fts_instr;
        struct stat *fts_statp;
        char fts_name[1];
} x86_ftsent_t;

void UnalignFTSENT(void* dest, void* source); // Arm -> x86
void AlignFTSENT(void* dest, void* source);   // x86 -> Arm

typedef struct my_flock64_s {
	uint16_t  l_type;
	uint16_t  l_whence;
	int64_t   l_start;
	int64_t   l_len;
	int       l_pid;
} my_flock64_t;

typedef struct __attribute__((packed)) x86_flock64_s {
	uint16_t  l_type;
	uint16_t  l_whence;
	int64_t   l_start;
	int64_t   l_len;
	int       l_pid;
} x86_flock64_t;

void UnalignFlock64(void* dest, void* source); // Arm -> x86
void AlignFlock64(void* dest, void* source);   // x86 -> Arm

// defined in wrapperlibc.c
int of_convert(int);    // x86->arm
int of_unconvert(int);  // arm->x86

typedef struct my_GValue_s
{
  int         g_type;
  union {
    int        v_int;
    int64_t    v_int64;
    uint64_t   v_uint64;
    float      v_float;
    double     v_double;
    void*      v_pointer;
  } data[2];
} my_GValue_t;

void alignNGValue(my_GValue_t* v, void* value, int n);
void unalignNGValue(void* value, my_GValue_t* v, int n);