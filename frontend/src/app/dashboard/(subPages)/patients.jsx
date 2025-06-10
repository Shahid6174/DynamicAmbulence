import Link from 'next/link';
export default function MainApp (props){

  return (
<div id="patients-view" className="view hidden">
  <div className="container-fluid">
    <div className="d-flex justify-content-between align-items-center mb-4">
      <h2>Patients</h2>
      <nav aria-label="breadcrumb">
        <ol className="breadcrumb">
          <li className="breadcrumb-item">
            <a href="#dashboard" data-route="dashboard">Dashboard</a>
          </li>
          <li className="breadcrumb-item active">Patients</li>
        </ol>
      </nav>
    </div>

    <div className="card mb-4">
      <div className="card-body">
        <div className="row">
          <div className="col-md-6">
            <div className="form-group">
              <input
                type="text"
                className="form-control"
                id="patient-search"
                placeholder="Search patients..."
              />
            </div>
          </div>
          <div className="col-md-4">
            <select className="form-control" id="patient-filter">
              <option value="">All Patients</option>
              <option value="Male">Male</option>
              <option value="Female">Female</option>
            </select>
          </div>
         
          <div className="col-md-2">
             <Link href="/add_patient">
                    <button
                      className="btn btn--primary btn--full-width">
                      <i className="bi bi-person-plus me-2"></i>Add New Patient
                      </button>
                   </Link>
          </div>
        </div>
      </div>
    </div>

    <div className="card">
      <div className="card-body">
        <div id="patients-list"></div>
      </div>
    </div>
  </div>
</div>

  );
};


