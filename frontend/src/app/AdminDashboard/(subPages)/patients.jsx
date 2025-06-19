'use client';

import Link from 'next/link';
import { useState, useEffect } from 'react';

export default function Patients({ patients = [] }) {
  const [search, setSearch] = useState('');
  const [filter, setFilter] = useState('');
  const [filteredPatients, setFilteredPatients] = useState(patients);

  useEffect(() => {
    let result = [...patients];

    if (search) {
      result = result.filter(p =>
        p.name.toLowerCase().includes(search.toLowerCase()) ||
        p.patientId?.toString().includes(search)
      );
    }

    if (filter) {
      result = result.filter(p => p.gender?.toLowerCase() === filter.toLowerCase());
    }

    setFilteredPatients(result);
  }, [search,filter]);

  return (
    <div id="patients-view" className="view">
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
                <input
                  type="text"
                  className="form-control"
                  id="patient-search"
                  placeholder="Search patients by name or ID..."
                  value={search}
                  onChange={(e) => setSearch(e.target.value)}
                />
              </div>
              <div className="col-md-4">
                <select
                  className="form-control"
                  id="patient-filter"
                  value={filter}
                  onChange={(e) => setFilter(e.target.value)}
                >
                  <option value="">All Genders</option>
                  <option value="Male">Male</option>
                  <option value="Female">Female</option>
                  <option value="Other">Other</option>
                </select>
              </div>
              <div className="col-md-2">
                <Link href="/AdminDashboard/add_patient">
                  <button className="btn btn-primary btn--full-width w-100">
                    <i className="bi bi-person-plus me-2"></i> Add New Patient
                  </button>
                </Link>
              </div>
            </div>
          </div>
        </div>

        <div className="card">
          <div className="card-body table-responsive">
            {filteredPatients.length > 0 ? (
              <table className="table table-striped">
                <thead>
                  <tr>
                    <th>Name</th>
                    <th>Patient ID</th>
                    <th>Age</th>
                    <th>Gender</th>
                    <th>Contact</th>
                    <th>Hospital</th>
                  </tr>
                </thead>
                <tbody>
                  {filteredPatients.map((p, index) => (
                    <tr key={index}>
                      <td>{p.name}</td>
                      <td>{p.patientId}</td>
                      <td>{p.age}</td>
                      <td>{p.gender || 'N/A'}</td>
                      <td>{p.phone || 'N/A'}</td>
                      <td>{p.hospital || 'N/A'}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            ) : (
              <p className="text-muted text-center mb-0">No patients found.</p>
            )}
          </div>
        </div>
      </div>
    </div>
  );
}
