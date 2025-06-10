import Link from 'next/link';

export default function DoctorsPage() {
  return (
    <div id="doctors-view" className="view">
      <div className="container-fluid">
        <div className="d-flex justify-content-between align-items-center mb-4">
          <h2>Doctors</h2>
          <nav aria-label="breadcrumb">
            <ol className="breadcrumb">
              <li className="breadcrumb-item">
                <Link href="/"><span>Dashboard</span></Link>
              </li>
              <li className="breadcrumb-item active">Doctors</li>
            </ol>
          </nav>
        </div>

        <div className="row mb-4">
          <div className="col-md-12">
            <div className="card shadow-sm">
              <div className="card-header bg-warning text-white d-flex align-items-center">
                <i className="bi bi-person-badge-fill me-2 display-6"></i>
                <h5 className="mb-0">Doctor Directory</h5>
              </div>
              <div className="card-body">
                <div id="doctors-list">
                  {/* Doctors list or table goes here */}
                  <p className="text-muted mb-0">No doctors available.</p>
                </div>
              </div>
            </div>
          </div>
        </div>

        <div className="d-flex gap-2">
          <Link href="/add_doctor">
            <button className="btn btn--primary">
              <i className="bi bi-person-plus me-2"></i>Add New Doctor
            </button>
          </Link>
        </div>
      </div>
    </div>
  );
}
