'use client';

import { useState } from 'react';

export default function ManageHospitals({ hospitals = [] }) {
  const [data, setData] = useState([
    {
      name: 'Fortis Hospital',
      area: 'Nagarbhavi',
      beds: 150,
      specialization: 'Cardiology',
    },
    {
      name: 'Aster CMI Hospital',
      area: 'Sahakarnagar',
      beds: 200,
      specialization: 'Neurology',
    },
  ]);

  return (
    <div id="manage-hospitals-tab" className="container-fluid py-4">
      <div className="d-flex justify-content-between align-items-center mb-4">
        <h4 className="fw-bold text-primary mb-0">Manage Hospitals</h4>
        <button className="btn btn-primary">
          <i className="bi bi-plus-lg me-2"></i>Add Hospital
        </button>
      </div>

      <div className="card shadow-sm">
        <div className="card-body table-responsive">
          {data.length === 0 ? (
            <p className="text-muted text-center mb-0">No hospitals found.</p>
          ) : (
            <table className="table table-hover align-middle">
              <thead className="table-light">
                <tr>
                  <th>#</th>
                  <th>Name</th>
                  <th>Area</th>
                  <th>Beds</th>
                  <th>Specialization</th>
                  <th>Rating</th>
                  <th className="text-end">Actions</th>
                </tr>
              </thead>
              <tbody>
                {data.map((h, idx) => (
                  <tr key={idx}>
                    <td>{idx + 1}</td>
                    <td>{h.name}</td>
                    <td>{h.area}</td>
                    <td>{h.beds}</td>
                    <td>{h.specialization}</td>
                    <td>
                      {Array(5)
                        .fill(0)
                        .map((_, i) => (
                          <i
                            key={i}
                            className="bi bi-star-fill text-warning"
                          ></i>
                        ))}
                    </td>
                    <td className="text-end">
                      <button className="btn btn-sm btn-outline-success me-2 p-1 ps-2">
                        <i className="bi bi-pencil-square me-1"></i>
                      </button>
                      <button className="btn btn-sm btn-outline-danger p-1 ps-2">
                        <i className="bi bi-trash me-1"></i>
                      </button>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          )}
        </div>
      </div>
    </div>
  );
}
